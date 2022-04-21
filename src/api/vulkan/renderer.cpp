#include "api/vulkan/renderer.h"

#include "core/command_buffer.h"
#include "core/command_pool.h"
#include "core/device_context.h"
#include "core/framebuffer.h"
#include "core/gpu_texture.h"
#include "core/gpu_buffer.h"
#include "core/rend_defs.h"
#include "core/render_pass.h"

#include "api/vulkan/fence.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/swapchain.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_semaphore.h"

#include <assert.h>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace rend;

Renderer::~Renderer(void)
{
    destroy();
}

Renderer& Renderer::instance(void)
{
    static Renderer s_renderer;
    return s_renderer;
}

StatusCode Renderer::create(const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues)
{
    if(initialised())
    {
        return StatusCode::ALREADY_CREATED;
    }

    auto& context = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    context.choose_gpu(desired_features);
    context.create_device(desired_queues);

    _swapchain = new Swapchain;
    _swapchain->create(3);

    _command_pool = new CommandPool;

    _create_default_depth_buffer(_swapchain->get_extent());
    _create_default_renderpass();
    _create_default_framebuffers(false);

    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        _frame_resources[idx].swapchain_idx = 0xdeadbeef;
        _frame_resources[idx].acquire_sem = new Semaphore;
        _frame_resources[idx].present_sem = new Semaphore;
        _frame_resources[idx].submit_fen  = new Fence;

        _frame_resources[idx].acquire_sem->create();
        _frame_resources[idx].present_sem->create();
        _frame_resources[idx].submit_fen->create(true);

        _frame_resources[idx].command_buffer = _command_pool->create_command_buffer();
    }

    create_resource();

    return StatusCode::SUCCESS;
}

StatusCode Renderer::destroy(void)
{
    if(!initialised())
    {
        return StatusCode::RESOURCE_NOT_CREATED;
    }

    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->wait_idle();

    while(!_task_queue.empty())
    {
        delete _task_queue.front();
        _task_queue.pop();
    }

    delete _default_depth_buffer;

    for(Framebuffer* framebuffer : _default_framebuffers)
    {
        delete framebuffer;
    }

    _default_render_pass->destroy();
    delete _default_render_pass;

    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        _frame_resources[idx].acquire_sem->destroy();
        delete _frame_resources[idx].acquire_sem;

        _frame_resources[idx].present_sem->destroy();
        delete _frame_resources[idx].present_sem;

        _frame_resources[idx].submit_fen->destroy();
        delete _frame_resources[idx].submit_fen;

        _command_pool->destroy_command_buffer(_frame_resources[idx].command_buffer);
    }

    delete _command_pool;

    _swapchain->destroy();
    delete _swapchain;

    destroy_resource();

    return StatusCode::SUCCESS;
}

Swapchain* Renderer::get_swapchain(void) const
{
    return _swapchain;
}

RenderPass* Renderer::get_default_render_pass(void) const
{
    return _default_render_pass;
}

void Renderer::load(GPUTexture* texture, ImageUsage type, const void* data, size_t bytes, uint32_t offset)
{
    ImageLoadTask* task = new ImageLoadTask(texture, type, data, bytes, offset);
    _task_queue.push(task);
}

void Renderer::load(GPUBuffer* buffer, BufferUsage type, const void* data, size_t bytes, uint32_t offset)
{
    BufferLoadTask* task = new BufferLoadTask(buffer, type, data, bytes, offset);
    _task_queue.push(task);
}

void Renderer::transition(GPUTexture* texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout)
{
    ImageTransitionTask* task = new ImageTransitionTask(texture, src, dst, final_layout);
    _task_queue.push(task);
}

FrameResources& Renderer::start_frame(void)
{
    FrameResources& frame_res = _frame_resources[_frame_counter % _FRAMES_IN_FLIGHT];
    _frame_counter++;
    frame_res.frame = _frame_counter;

    frame_res.submit_fen->wait();
    frame_res.command_buffer->reset();

    StatusCode code = StatusCode::SUCCESS;
    while((code = _swapchain->acquire(frame_res.acquire_sem, nullptr)) != StatusCode::SUCCESS)
    {
        if(code == StatusCode::SWAPCHAIN_ACQUIRE_ERROR)
        {
            std::cerr << "Failed to acquire swapchain image" << std::endl;
            std::abort();
        }

        if(code == StatusCode::SWAPCHAIN_OUT_OF_DATE)
            resize_resources();
    }

    frame_res.swapchain_idx = _swapchain->get_current_image_index();
    frame_res.framebuffer = _default_framebuffers[frame_res.swapchain_idx];

    _process_task_queue(frame_res);

    return frame_res;
}

void Renderer::end_frame(FrameResources& frame_res)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    frame_res.submit_fen->reset();
    VkCommandBuffer vk_command_buffer = ctx.get_command_buffer(frame_res.command_buffer->handle());
    ctx.get_device()->queue_submit(&vk_command_buffer, 1, QueueType::GRAPHICS, { frame_res.acquire_sem }, { frame_res.present_sem }, frame_res.submit_fen);
    _swapchain->present(QueueType::GRAPHICS, { frame_res.present_sem });
}

void Renderer::resize_resources(void)
{
    _swapchain->recreate();
    _create_default_depth_buffer(_swapchain->get_extent());
    _create_default_framebuffers(true);
}

void Renderer::_process_task_queue(FrameResources& resources)
{
    if(_task_queue.empty())
        return;

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    Fence load_fence;
    load_fence.create(false);

    resources.command_buffer->begin();

    while(!_task_queue.empty())
    {
        Task* task = _task_queue.front();
        _task_queue.pop();
        task->execute(resources);
        delete task;
    }

    resources.command_buffer->end();

    if(resources.command_buffer->recorded())
    {
        VkCommandBuffer vk_command_buffer = ctx.get_command_buffer(resources.command_buffer->handle());
        ctx.get_device()->queue_submit(&vk_command_buffer, 1, QueueType::GRAPHICS, {}, {}, &load_fence);
        load_fence.wait();
    }

    resources.command_buffer->reset();
    load_fence.destroy();

    for(auto staging_buffer : resources.staging_buffers)
    {
        delete staging_buffer;
    }

    resources.staging_buffers.clear();
}

void ImageLoadTask::execute(FrameResources& resources)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    void* mapped = NULL;

    BufferInfo info{ 1, size_bytes, BufferUsage::UNIFORM_BUFFER };
    GPUBuffer* staging_buffer = new GPUBuffer(info);
    resources.staging_buffers.push_back(staging_buffer);

#ifdef DEBUG
    std::stringstream dbg_sstream;
    dbg_sstream << "Frame #" << resources.frame << " Staging Buffer #" << resources.staging_buffers.size();
    staging_buffer->dbg_name(dbg_sstream.str());
#endif

    mapped = ctx.map_buffer_memory(staging_buffer->handle(), staging_buffer->bytes());
    memcpy(mapped, data, size_bytes);
    ctx.unmap_buffer_memory(staging_buffer->handle());

    resources.command_buffer->copy(*staging_buffer, *image);
}

void BufferLoadTask::execute(FrameResources& resources)
{
    // TODO: This should be based on the memory properties, not the resource
    bool is_device_local{ false };
    if ((buffer_usage & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE ||
        (buffer_usage & BufferUsage::INDEX_BUFFER)  != BufferUsage::NONE)
    {
        is_device_local = true;
    }

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    void* mapped = NULL;

    if(is_device_local)
    {
        BufferInfo info{ 1, size_bytes, BufferUsage::UNIFORM_BUFFER };
        GPUBuffer* staging_buffer = new GPUBuffer(info);
        resources.staging_buffers.push_back(staging_buffer);

#ifdef DEBUG
        std::stringstream dbg_sstream;
        dbg_sstream << "Frame #" << resources.frame << " Staging Buffer #" << resources.staging_buffers.size();
        staging_buffer->dbg_name(dbg_sstream.str());
#endif

        mapped = ctx.map_buffer_memory(staging_buffer->handle(), staging_buffer->bytes());
        memcpy(mapped, data, size_bytes);
        ctx.unmap_buffer_memory(staging_buffer->handle());

        resources.command_buffer->copy(*staging_buffer, *buffer);
    }
    else
    {
        mapped = ctx.map_buffer_memory(buffer->handle(), buffer->bytes());
        memcpy(mapped, data, size_bytes);
        ctx.unmap_buffer_memory(buffer->handle());
    }
}

void ImageTransitionTask::execute(FrameResources& resources)
{
    resources.command_buffer->transition_image(*image, src, dst, final_layout);
}

void Renderer::_create_default_depth_buffer(VkExtent2D extent)
{
    if(_default_depth_buffer)
    {
        delete _default_depth_buffer;
    }

    TextureInfo info{ extent.width, extent.height, 1, 1, 1, Format::D24_S8, ImageLayout::UNDEFINED,  MSAASamples::MSAA_1X, ImageUsage::DEPTH_STENCIL };
    _default_depth_buffer = new GPUTexture(info);
}

void Renderer::_create_default_renderpass(void)
{
    _default_render_pass = new RenderPass;

    AttachmentInfo colour_attachment = {};
    colour_attachment.format = _swapchain->get_format();
    colour_attachment.load_op = LoadOp::CLEAR;
    colour_attachment.store_op = StoreOp::STORE;
    colour_attachment.final_layout = ImageLayout::PRESENT;

    AttachmentInfo depth_stencil_attachment = {};
    depth_stencil_attachment.format = _default_depth_buffer->format();
    depth_stencil_attachment.samples = _default_depth_buffer->samples();
    depth_stencil_attachment.load_op = LoadOp::CLEAR;
    depth_stencil_attachment.store_op = StoreOp::STORE;
    depth_stencil_attachment.final_layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT;

    SubpassInfo subpass_info = {};
    subpass_info.colour_attachment_infos[0] = 0;
    subpass_info.depth_stencil_attachment   = 1;
    subpass_info.colour_attachment_infos_count = 1;

    SubpassDependency dep_info = {};
    dep_info.src_sync = Synchronisation{ PipelineStage::PIPELINE_STAGE_BOTTOM_OF_PIPE, MemoryAccess::MEMORY_READ };
    dep_info.dst_sync = Synchronisation{ PipelineStage::PIPELINE_STAGE_COLOUR_OUTPUT,  MemoryAccess::COLOUR_ATTACHMENT_READ | MemoryAccess::COLOUR_ATTACHMENT_WRITE };

    RenderPassInfo render_pass_info = {};
    render_pass_info.attachment_infos[0] = colour_attachment;
    render_pass_info.attachment_infos[1] = depth_stencil_attachment;
    render_pass_info.attachment_infos_count = 2;
    render_pass_info.subpasses[0] = subpass_info;
    render_pass_info.subpasses_count = 1;
    render_pass_info.subpass_dependencies[0] = dep_info;
    render_pass_info.subpass_dependency_count = 1;

    _default_render_pass->create(render_pass_info);
}

void Renderer::_create_default_framebuffers(bool recreate)
{
    const std::vector<Texture2DHandle> targets = _swapchain->get_back_buffer_handles();

    VkExtent2D swapchain_extent = _swapchain->get_extent();

    if(recreate)
    {
        for(uint32_t idx = 0; idx < _default_framebuffers.size(); ++idx)
        {
            delete _default_framebuffers[idx];
        }
    }

    _default_framebuffers.resize(targets.size());
    for(uint32_t idx = 0; idx < _default_framebuffers.size(); ++idx)
    {
        FramebufferInfo fb_info = {};
        fb_info.width  = swapchain_extent.width;
        fb_info.height = swapchain_extent.height;
        fb_info.depth  = 1;
        fb_info.render_pass_handle = _default_render_pass->handle();
        fb_info.depth_buffer_handle = _default_depth_buffer->handle();
        fb_info.render_target_handles[0] = targets[idx];
        fb_info.render_target_handles_count = 1;

        _default_framebuffers[idx] = new Framebuffer(fb_info);
    }
}
