#include "renderer.h"

#include "command_pool.h"
#include "command_buffer.h"
#include "device_context.h"
#include "fence.h"
#include "framebuffer.h"
#include "gpu_texture.h"
#include "gpu_buffer.h"
#include "logical_device.h"
#include "rend_defs.h"
#include "render_pass.h"
#include "semaphore.h"
#include "swapchain.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_instance.h"
#include "vulkan_device_context.h"

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
    _command_pool->create(context.get_device()->get_queue_family(QueueType::GRAPHICS), true);

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
        _frame_resources[idx].command_buffer = _command_pool->allocate_command_buffer();
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
        framebuffer->destroy();
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
    }

    _command_pool->destroy();
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

void Renderer::load(GPUTexture* texture, ImageUsage type, void* data, size_t bytes, uint32_t offset)
{
    ImageLoadTask* task = new ImageLoadTask(texture, type, data, bytes, offset);
    _task_queue.push(task);
}

void Renderer::load(GPUBuffer* buffer, BufferUsage type, void* data, size_t bytes, uint32_t offset)
{
    BufferLoadTask* task = new BufferLoadTask(buffer, type, data, bytes, offset);
    _task_queue.push(task);
}

void Renderer::transition(GPUTexture* texture, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkImageLayout final_layout)
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
    frame_res.submit_fen->reset();
    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->queue_submit({ frame_res.command_buffer }, QueueType::GRAPHICS, { frame_res.acquire_sem }, { frame_res.present_sem }, frame_res.submit_fen);
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
        static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->queue_submit({ resources.command_buffer }, QueueType::GRAPHICS, {}, {}, &load_fence);
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
    GPUBuffer* staging_buffer = new GPUBuffer;
#ifdef DEBUG
    std::stringstream dbg_sstream;
    dbg_sstream << "Frame #" << resources.frame << " Staging Buffer #" << resources.staging_buffers.size();
    staging_buffer->dbg_name(dbg_sstream.str());
#endif
    staging_buffer->create(info);
    resources.staging_buffers.push_back(staging_buffer);

    VkDeviceMemory memory = ctx.get_memory(staging_buffer->get_handle());
    ctx.get_device()->map_memory(memory, staging_buffer->bytes(), 0, &mapped);
    memcpy(mapped, data, size_bytes);
    ctx.get_device()->unmap_memory(memory);

    resources.command_buffer->copy_buffer_to_image(*staging_buffer, *image);
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
    VkDeviceMemory memory = VK_NULL_HANDLE;
    void* mapped = NULL;

    if(is_device_local)
    {
        BufferInfo info{ 1, size_bytes, BufferUsage::UNIFORM_BUFFER };
        GPUBuffer* staging_buffer = new GPUBuffer;
#ifdef DEBUG
        std::stringstream dbg_sstream;
        dbg_sstream << "Frame #" << resources.frame << " Staging Buffer #" << resources.staging_buffers.size();
        staging_buffer->dbg_name(dbg_sstream.str());
#endif
        staging_buffer->create(info);

        resources.staging_buffers.push_back(staging_buffer);

        memory = ctx.get_memory(staging_buffer->get_handle());

        ctx.get_device()->map_memory(memory, staging_buffer->bytes(), 0, &mapped);
        memcpy(mapped, data, size_bytes);
        ctx.get_device()->unmap_memory(memory);

        resources.command_buffer->copy_buffer_to_buffer(*staging_buffer, *buffer);
    }
    else
    {
        memory = ctx.get_memory(buffer->get_handle());

        ctx.get_device()->map_memory(memory, size_bytes, 0, &mapped);
        memcpy(mapped, data, size_bytes);
        ctx.get_device()->unmap_memory(memory);
    }
}

void ImageTransitionTask::execute(FrameResources& resources)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    std::vector<VkImageMemoryBarrier> barriers(1);
    VkImageMemoryBarrier* barrier = barriers.data();

    *barrier = VkImageMemoryBarrier
    {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = 0,
        .dstAccessMask       = 0,
        .oldLayout           = vulkan_helpers::convert_image_layout(image->layout()),
        .newLayout           = final_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = ctx.get_image(image->get_handle()),
        .subresourceRange    =
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#endif

    switch(vulkan_helpers::convert_image_layout(image->layout()))
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier->srcAccessMask = 0; break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            barrier->srcAccessMask = VK_ACCESS_HOST_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier->srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier->srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier->srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier->srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier->srcAccessMask = VK_ACCESS_SHADER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_GENERAL:
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
        case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV:
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
        case VK_IMAGE_LAYOUT_RANGE_SIZE:
        case VK_IMAGE_LAYOUT_MAX_ENUM:
            std::cerr << "Transition: old layout is unsupported" << std::endl;
            return;
    }

    switch(final_layout)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier->dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier->dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier->dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            if(barrier->srcAccessMask == 0)
                barrier->srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier->dstAccessMask = VK_ACCESS_SHADER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_UNDEFINED:
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
        case VK_IMAGE_LAYOUT_GENERAL:
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
        case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV:
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
        case VK_IMAGE_LAYOUT_RANGE_SIZE:
        case VK_IMAGE_LAYOUT_MAX_ENUM:
            std::cerr << "Transition: new layout is unsupported" << std::endl;
            return;
    }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    resources.command_buffer->pipeline_barrier(src, dst, VK_DEPENDENCY_BY_REGION_BIT, {}, {}, barriers);

    image->layout(vulkan_helpers::convert_image_layout(final_layout));
}

void Renderer::_create_default_depth_buffer(VkExtent2D extent)
{
    if(_default_depth_buffer)
        delete _default_depth_buffer;

    TextureInfo info{ extent.width, extent.height, 1, 1, 1, Format::D24_S8, ImageLayout::UNDEFINED,  MSAASamples::MSAA_1X, ImageUsage::DEPTH_STENCIL };
    _default_depth_buffer = new GPUTexture;
    _default_depth_buffer->create(info);
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
    dep_info.src_sync = Synchronisation{ PipelineStage::BOTTOM_OF_PIPE, MemoryAccess::MEMORY_READ };
    dep_info.dst_sync = Synchronisation{ PipelineStage::COLOUR_OUTPUT,  MemoryAccess::COLOUR_ATTACHMENT_READ | MemoryAccess::COLOUR_ATTACHMENT_WRITE };

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
            _default_framebuffers[idx]->destroy();
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
        fb_info.depth_buffer_handle = _default_depth_buffer->get_handle();
        fb_info.render_target_handles[0] = targets[idx];
        fb_info.render_target_handles_count = 1;

        _default_framebuffers[idx] = new Framebuffer;
        _default_framebuffers[idx]->create(fb_info);
    }
}
