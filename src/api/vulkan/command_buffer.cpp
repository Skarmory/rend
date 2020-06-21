#include "command_buffer.h"

#include "descriptor_set.h"
#include "framebuffer.h"
#include "vulkan_gpu_buffer.h"
#include "vulkan_gpu_texture.h"
#include "pipeline.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

#include "gpu_buffer_base.h"
#include "index_buffer.h"
#include "vertex_buffer.h"

#include <iostream>

using namespace rend;

CommandBuffer::CommandBuffer(CommandPool* pool, VkCommandBuffer vk_command_buffer)
    : _pool(pool),
      _recording(false),
      _recorded(false),
      _vk_command_buffer(vk_command_buffer)
{
}

CommandBuffer::~CommandBuffer(void)
{
}

CommandPool& CommandBuffer::get_pool(void) const
{
    return *_pool;
}

VkCommandBuffer CommandBuffer::get_handle(void) const
{
    return _vk_command_buffer;
}

bool CommandBuffer::begin(void)
{
    VkCommandBufferBeginInfo info =
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .pInheritanceInfo = nullptr
    };

    if(vkBeginCommandBuffer(_vk_command_buffer, &info) != VK_SUCCESS)
        return true;

    _recording = true;

    return false;
}

bool CommandBuffer::end(void)
{
    if(vkEndCommandBuffer(_vk_command_buffer) != VK_SUCCESS)
        return false;

    _recording = false;

    return true;
}

bool CommandBuffer::reset(void)
{
    if(vkResetCommandBuffer(_vk_command_buffer, 0) != VK_SUCCESS)
        return false;

    _recorded = false;

    return true;
}

bool CommandBuffer::recording(void) const
{
    return _recording;
}

bool CommandBuffer::recorded(void) const
{
    return _recorded;
}

void CommandBuffer::begin_render_pass(const RenderPass& render_pass, const Framebuffer& framebuffer, VkRect2D render_area, const std::vector<VkClearValue>& clear_values)
{
    _recorded = true;

    VkRenderPassBeginInfo info =
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = render_pass.get_handle(),
        .framebuffer = framebuffer.get_handle(),
        .renderArea = render_area,
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data()
    };

    vkCmdBeginRenderPass(_vk_command_buffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::end_render_pass(RenderPass& render_pass, Framebuffer& framebuffer)
{
    UU(render_pass);

    _recorded = true;

    vkCmdEndRenderPass(_vk_command_buffer);

    framebuffer.on_end_render_pass();
}

void CommandBuffer::set_viewport(const VkViewport& viewport)
{
    _recorded = true;

    vkCmdSetViewport(_vk_command_buffer, 0, 1, &viewport);
}

void CommandBuffer::set_scissors(const VkRect2D& scissor)
{
    _recorded = true;

    vkCmdSetScissor(_vk_command_buffer, 0, 1, &scissor);
}

void CommandBuffer::bind_pipeline(VkPipelineBindPoint bind_point, const Pipeline& pipeline)
{
    _recorded = true;

    vkCmdBindPipeline(_vk_command_buffer, bind_point, pipeline.get_handle());
}

void CommandBuffer::bind_descriptor_sets(VkPipelineBindPoint bind_point, const PipelineLayout& layout, const std::vector<DescriptorSet*>& sets)
{
    _recorded = true;

    std::vector<VkDescriptorSet> vk_sets;
    vk_sets.reserve(sets.size());

    for(DescriptorSet* dset : sets)
        vk_sets.push_back(dset->get_handle());

    vkCmdBindDescriptorSets(_vk_command_buffer, bind_point, layout.get_handle(), 0, static_cast<uint32_t>(vk_sets.size()), vk_sets.data(), 0, nullptr);
}

void CommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    _recorded = true;

    vkCmdDraw(_vk_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
    _recorded = true;

    vkCmdDrawIndexed(_vk_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void CommandBuffer::bind_index_buffer(const IndexBuffer& buffer, VkDeviceSize offset, VkIndexType index_type)
{
    _recorded = true;

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    vkCmdBindIndexBuffer(_vk_command_buffer,  ctx.get_buffer(buffer.get_handle()), offset, index_type);
}

void CommandBuffer::bind_vertex_buffers(uint32_t first_binding, const std::vector<VulkanGPUBuffer*>& buffers, const std::vector<VkDeviceSize>& offsets)
{
    _recorded = true;

    std::vector<VkBuffer> vk_buffers;
    vk_buffers.reserve(buffers.size());

    for(VulkanGPUBuffer* buf : buffers)
        vk_buffers.push_back(buf->get_handle());

    vkCmdBindVertexBuffers(_vk_command_buffer, first_binding, static_cast<uint32_t>(buffers.size()), vk_buffers.data(), offsets.data());
}

void CommandBuffer::bind_vertex_buffers(uint32_t first_binding, const std::vector<VertexBuffer*>& buffers, const std::vector<VkDeviceSize>& offsets)
{
    _recorded = true;

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    std::vector<VkBuffer> vk_buffers;
    vk_buffers.reserve(buffers.size());

    for (auto* buf : buffers)
    {
        vk_buffers.push_back(ctx.get_buffer(buf->get_handle()));
    }

    vkCmdBindVertexBuffers(_vk_command_buffer, first_binding, static_cast<uint32_t>(buffers.size()), vk_buffers.data(), offsets.data());
}

void CommandBuffer::push_constant(const PipelineLayout& layout, VkShaderStageFlags shader_stages, uint32_t offset, uint32_t size, const void* data)
{
    _recorded = true;

    vkCmdPushConstants(_vk_command_buffer, layout.get_handle(), shader_stages, offset, size, data);
}

void CommandBuffer::copy_buffer_to_image(const GPUBufferBase& buffer, const VulkanGPUTexture& image)
{
    _recorded = true;

    VkBufferImageCopy copy =
    {
        .bufferOffset           = 0,
        .bufferRowLength        = image.get_extent().width,
        .bufferImageHeight      = image.get_extent().height,
        .imageSubresource       =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = image.get_array_layers()
            },
        .imageOffset            = { 0, 0, 0 },
        .imageExtent            = image.get_extent()
    };

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    vkCmdCopyBufferToImage(_vk_command_buffer, ctx.get_buffer(buffer.get_handle()), image.get_handle(), image.get_layout(), 1, &copy);
}

void CommandBuffer::copy_buffer_to_buffer(const GPUBufferBase& src, const GPUBufferBase& dst)
{
    _recorded = true;

    VkBufferCopy copy =
    {
        .srcOffset = 0,
        .dstOffset = 0,
        .size      = src.bytes()
    };

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    vkCmdCopyBuffer(_vk_command_buffer, ctx.get_buffer(src.get_handle()), ctx.get_buffer(dst.get_handle()), 1, &copy);
}

void CommandBuffer::blit_image(const VulkanGPUTexture& src, const VulkanGPUTexture& dst)
{
    _recorded = true;

    VkImageBlit blit = {};
    blit.srcSubresource.aspectMask     = vulkan_helpers::find_image_aspects(src.get_vk_format());
    blit.srcSubresource.mipLevel       = 0;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount     = src.get_array_layers();

    blit.srcOffsets[0].x = 0;
    blit.srcOffsets[0].y = 0;
    blit.srcOffsets[0].z = 0;
    blit.srcOffsets[1].x = src.width();
    blit.srcOffsets[1].y = src.height();
    blit.srcOffsets[1].z = 1;

    blit.dstSubresource.aspectMask     = vulkan_helpers::find_image_aspects(dst.get_vk_format());
    blit.dstSubresource.mipLevel       = 0;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount     = dst.get_array_layers();

    blit.dstOffsets[0].x = 0;
    blit.dstOffsets[0].y = 0;
    blit.dstOffsets[0].z = 0;
    blit.dstOffsets[1].x = dst.width();
    blit.dstOffsets[1].y = dst.height();
    blit.dstOffsets[1].z = 1;

    vkCmdBlitImage(_vk_command_buffer, src.get_handle(), src.get_layout(), dst.get_handle(), dst.get_layout(), 1, &blit, VK_FILTER_LINEAR);
}

void CommandBuffer::transition_image(VulkanGPUTexture& image, VkImageLayout transition_to, VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage)
{
    std::vector<VkImageMemoryBarrier> barriers(1);
    VkImageMemoryBarrier& barrier = barriers[0];

    barrier = VkImageMemoryBarrier
    {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = 0,
        .dstAccessMask       = 0,
        .oldLayout           = image.get_layout(),
        .newLayout           = transition_to,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image.get_handle(),
        .subresourceRange    =
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    switch(image.get_layout())
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier.srcAccessMask = 0; break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT; break;
        case VK_IMAGE_LAYOUT_GENERAL:
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
        case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV:
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
        case VK_IMAGE_LAYOUT_RANGE_SIZE:
        case VK_IMAGE_LAYOUT_MAX_ENUM:
            std::cerr << "Image transition error: src layout " << vulkan_helpers::stringify(image.get_layout()) << " is not supported" << std::endl;
            return;
    }

    switch(transition_to)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            if(barrier.srcAccessMask == 0)
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT; break;
        case VK_IMAGE_LAYOUT_UNDEFINED:
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
        case VK_IMAGE_LAYOUT_GENERAL:
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV:
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
        case VK_IMAGE_LAYOUT_RANGE_SIZE:
        case VK_IMAGE_LAYOUT_MAX_ENUM:
            std::cerr << "Image transition error: dst layout " << vulkan_helpers::stringify(transition_to) << " is not supported" << std::endl;
            return;
    }

    pipeline_barrier(src_stage, dst_stage, VK_DEPENDENCY_BY_REGION_BIT, {}, {}, barriers);

    image.transition(transition_to);
}

void CommandBuffer::pipeline_barrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkDependencyFlags dependency, const std::vector<VkMemoryBarrier>& memory_barriers, const std::vector<VkBufferMemoryBarrier>& buffer_memory_barriers, const std::vector<VkImageMemoryBarrier>& image_memory_barriers)
{
    _recorded = true;

    vkCmdPipelineBarrier(_vk_command_buffer, src, dst, dependency, static_cast<uint32_t>(memory_barriers.size()), memory_barriers.data(), static_cast<uint32_t>(buffer_memory_barriers.size()), buffer_memory_barriers.data(), static_cast<uint32_t>(image_memory_barriers.size()), image_memory_barriers.data());
}
