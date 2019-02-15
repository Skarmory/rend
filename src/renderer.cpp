#include "renderer.h"

#include "buffer.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "device_context.h"
#include "fence.h"
#include "framebuffer.h"
#include "gpu_buffer.h"
#include "image.h"
#include "logical_device.h"
#include "semaphore.h"
#include "swapchain.h"
#include "texture2D.h"

#include <assert.h>
#include <cstring>
#include <iostream>

using namespace rend;

Renderer::Renderer(Window* window, const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues, std::vector<const char*> extensions, std::vector<const char*> layers)
    : _frame_counter(0)
{
    _context = new DeviceContext(extensions.data(), extensions.size(), layers.data(), layers.size(), window);
    _context->create_device(desired_features, desired_queues);
    _swapchain = _context->get_device()->create_swapchain(3);
    _command_pool = _context->get_device()->create_command_pool(QueueType::GRAPHICS, true);

    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        _frame_resources[idx].acquire_sem = _context->get_device()->create_semaphore();
        _frame_resources[idx].present_sem = _context->get_device()->create_semaphore();
        _frame_resources[idx].submit_fen  = _context->get_device()->create_fence(true);
        _frame_resources[idx].command_buffer = _command_pool->allocate_command_buffer();
    }

    _create_default_renderpass();
    _create_default_framebuffers(false);
}

Renderer::~Renderer(void)
{
    vkDeviceWaitIdle(_context->get_device()->get_handle());

    while(!_task_queue.empty())
    {
        delete _task_queue.front();
        _task_queue.pop();
    }

    _context->get_device()->destroy_image(&_default_depth_buffer);

    for(Framebuffer* framebuffer : _default_framebuffers)
        _context->get_device()->destroy_framebuffer(&framebuffer);

    _context->get_device()->destroy_render_pass(&_default_render_pass);

    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        _context->get_device()->destroy_semaphore(&_frame_resources[idx].acquire_sem);
        _context->get_device()->destroy_semaphore(&_frame_resources[idx].present_sem);
        _context->get_device()->destroy_fence(&_frame_resources[idx].submit_fen);
    }

    _context->get_device()->destroy_command_pool(&_command_pool);
    _context->get_device()->destroy_swapchain(&_swapchain);

    delete _context;
}

DeviceContext* Renderer::get_device_context(void) const
{
    return _context;
}

Swapchain* Renderer::get_swapchain(void) const
{
    return _swapchain;
}

RenderPass* Renderer::get_default_render_pass(void) const
{
    return _default_render_pass;
}

Buffer* Renderer::create_vertex_buffer(size_t vertex_count, size_t vertex_size)
{
    return new Buffer(_context, vertex_count * vertex_size, BufferType::VERTEX);
}

Buffer* Renderer::create_index_buffer(size_t index_count, size_t index_size)
{
    return new Buffer(_context, index_count * index_size, BufferType::INDEX);
}

Buffer* Renderer::create_uniform_buffer(size_t size_bytes)
{
    return new Buffer(_context, size_bytes, BufferType::UNIFORM);
}

Texture2D* Renderer::create_diffuse(uint32_t width, uint32_t height, uint32_t mip_levels)
{
    return new Texture2D(_context, width, height, mip_levels, TextureType::DIFFUSE);
}

void Renderer::load(Texture2D* texture, void* data, size_t size_bytes)
{
    LoadTask* task = new LoadTask;
    task->resource_type = ResourceType::TEXTURE2D;
    task->resource      = texture;
    task->data          = data;
    task->size_bytes    = size_bytes;

    _task_queue.push(task);
}

void Renderer::load(Buffer* buffer, void* data, size_t size_bytes)
{
    LoadTask* task      = new LoadTask;
    task->resource_type = ResourceType::BUFFER;
    task->resource      = buffer;
    task->data          = data;
    task->size_bytes    = size_bytes;

    _task_queue.push(task);
}

void Renderer::transition(Texture2D* texture, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkImageLayout final_layout)
{
    ImageTransitionTask* task = new ImageTransitionTask;
    task->image        = texture;
    task->src          = src;
    task->dst          = dst;
    task->final_layout = final_layout;

    _task_queue.push(task);
}

FrameResources* Renderer::start_frame(void)
{
    FrameResources* frame_res = &_frame_resources[_frame_counter % _FRAMES_IN_FLIGHT];
    _frame_counter++;

    frame_res->submit_fen->wait();
    frame_res->submit_fen->reset();
    frame_res->command_buffer->reset();

    do
    {
        if((frame_res->swapchain_idx = _swapchain->acquire(frame_res->acquire_sem, nullptr)) == SWAPCHAIN_OUT_OF_DATE)
            resize_resources();
    }
    while(frame_res->swapchain_idx == SWAPCHAIN_OUT_OF_DATE);

    _process_task_queue(frame_res);

    return frame_res;
}

void Renderer::end_frame(FrameResources* frame_res)
{
    _swapchain->present(QueueType::GRAPHICS, { frame_res->present_sem });
}

void Renderer::begin_render_pass(FrameResources* frame_res, std::vector<VkClearValue>& clear_values, VkRect2D render_area)
{
    if(render_area.extent.width == 0 && render_area.extent.height == 0)
        render_area.extent = _swapchain->get_extent();

    frame_res->command_buffer->begin_render_pass(*_default_render_pass, _default_framebuffers[frame_res->swapchain_idx], render_area, clear_values);
}

void Renderer::end_render_pass(FrameResources* frame_res)
{
    frame_res->command_buffer->end_render_pass();
}

void Renderer::resize_resources(void)
{
    _swapchain->recreate();
    _create_default_framebuffers(true);
}

void Renderer::_process_task_queue(FrameResources* resources)
{
    if(_task_queue.empty())
        return;

    Fence* load_fence = _context->get_device()->create_fence(false);

    resources->command_buffer->begin();

    while(!_task_queue.empty())
    {
        Task* task = _task_queue.front();
        _task_queue.pop();
        task->execute(_context, resources);
        delete task;
    }

    resources->command_buffer->end();

    if(resources->command_buffer->recorded())
    {
        _context->get_device()->queue_submit({ resources->command_buffer }, QueueType::GRAPHICS, {}, {}, load_fence);
        load_fence->wait();
    }

    resources->command_buffer->reset();
    _context->get_device()->destroy_fence(&load_fence);

    for(auto staging_buffer : resources->staging_buffers)
    {
        _context->get_device()->destroy_buffer(&staging_buffer);
    }

    resources->staging_buffers.clear();
}

void LoadTask::execute(DeviceContext* context, FrameResources* resources)
{
    bool is_device_local;
    switch(resource_type)
    {
        case ResourceType::BUFFER:
            is_device_local = static_cast<Buffer*>(resource)->get_gpu_buffer()->get_memory_properties() & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case ResourceType::TEXTURE2D:
            is_device_local = static_cast<Texture2D*>(resource)->get_image()->get_memory_properties() & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
    }

    VkDeviceMemory memory;
    void* mapped;

    if(is_device_local)
    {
        GPUBuffer* staging_buffer = context->get_device()->create_buffer(size_bytes, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        resources->staging_buffers.push_back(staging_buffer);

        memory = staging_buffer->get_memory();

        vkMapMemory(context->get_device()->get_handle(), memory, 0, staging_buffer->get_size(), 0, &mapped);
        memcpy(mapped, data, size_bytes);
        vkUnmapMemory(context->get_device()->get_handle(), memory);

        switch(resource_type)
        {
            case ResourceType::BUFFER:
                resources->command_buffer->copy_buffer_to_buffer(staging_buffer, static_cast<Buffer*>(resource)->get_gpu_buffer());
                break;
            case ResourceType::TEXTURE2D:
                resources->command_buffer->copy_buffer_to_image(staging_buffer, static_cast<Texture2D*>(resource)->get_image());
                break;
        }
    }
    else
    {
        switch(resource_type)
        {
            case ResourceType::BUFFER:
                memory = static_cast<Buffer*>(resource)->get_gpu_buffer()->get_memory();
                break;
            case ResourceType::TEXTURE2D:
                memory = static_cast<Texture2D*>(resource)->get_image()->get_memory();
                break;
        }

        vkMapMemory(context->get_device()->get_handle(), memory, 0, size_bytes, 0, &mapped);
        memcpy(mapped, data, size_bytes);
        vkUnmapMemory(context->get_device()->get_handle(), memory);
    }
}

void ImageTransitionTask::execute(DeviceContext* context, FrameResources* resources)
{
    UU(context);

    std::vector<VkImageMemoryBarrier> barriers(1);
    VkImageMemoryBarrier* barrier = barriers.data();

    *barrier = VkImageMemoryBarrier
    {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = 0,
        .dstAccessMask       = 0,
        .oldLayout           = image->get_image()->_vk_layout,
        .newLayout           = final_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image->get_image()->get_handle(),
        .subresourceRange    =
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    switch(image->get_image()->_vk_layout)
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

    resources->command_buffer->pipeline_barrier(src, dst, VK_DEPENDENCY_BY_REGION_BIT, {}, {}, barriers);

    image->get_image()->_vk_layout = final_layout;
}

void Renderer::_create_default_renderpass(void)
{
    std::vector<VkAttachmentDescription> attach_descs(2);
    attach_descs[0].format        = VK_FORMAT_B8G8R8A8_UNORM;
    attach_descs[0].loadOp        = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attach_descs[0].storeOp       = VK_ATTACHMENT_STORE_OP_STORE;
    attach_descs[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attach_descs[0].finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attach_descs[0].samples       = VK_SAMPLE_COUNT_1_BIT;

    attach_descs[1].format         = VK_FORMAT_D24_UNORM_S8_UINT;
    attach_descs[1].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attach_descs[1].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    attach_descs[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attach_descs[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    attach_descs[1].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attach_descs[1].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attach_descs[1].samples        = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkAttachmentReference> attachment_refs(2);
    attachment_refs[0].attachment = 0;
    attachment_refs[0].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachment_refs[1].attachment = 1;
    attachment_refs[1].layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::vector<VkSubpassDescription> subpass_descs(1);
    subpass_descs[0].pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_descs[0].colorAttachmentCount    = 1;
    subpass_descs[0].pColorAttachments       = &attachment_refs[0];
    subpass_descs[0].pDepthStencilAttachment = &attachment_refs[1];

    std::vector<VkSubpassDependency> subpass_deps(2);
    subpass_deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_deps[0].dstSubpass = 0;
    subpass_deps[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpass_deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_deps[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpass_deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_deps[1].srcSubpass = 0;
    subpass_deps[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpass_deps[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_deps[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpass_deps[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_deps[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    _default_render_pass = _context->get_device()->create_render_pass(attach_descs, subpass_descs, subpass_deps);
}

void Renderer::_create_default_framebuffers(bool recreate)
{
    const std::vector<VkImageView>& views = _swapchain->get_image_views();

    VkExtent2D swapchain_extent = _swapchain->get_extent();
    VkExtent3D framebuffer_dims = { swapchain_extent.width, swapchain_extent.height, 1 };

    if(recreate)
    {
        _context->get_device()->destroy_image(&_default_depth_buffer);
        _default_depth_buffer = _context->get_device()->create_image(framebuffer_dims, VK_IMAGE_TYPE_2D, VK_FORMAT_D24_UNORM_S8_UINT, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

        for(uint32_t idx = 0; idx < _default_framebuffers.size(); ++idx)
        {
            _default_framebuffers[idx]->recreate({ views[idx], _default_depth_buffer->get_view() }, framebuffer_dims);
        }


    }
    else
    {
        _default_depth_buffer = _context->get_device()->create_image(framebuffer_dims, VK_IMAGE_TYPE_2D, VK_FORMAT_D24_UNORM_S8_UINT, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

        _default_framebuffers.resize(views.size());
        for(uint32_t idx = 0; idx < _default_framebuffers.size(); ++idx)
        {
            _default_framebuffers[idx] = _context->get_device()->create_framebuffer(*_default_render_pass, { views[idx], _default_depth_buffer->get_view() }, framebuffer_dims);
        }
    }
}
