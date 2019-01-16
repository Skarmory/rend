#include "command_buffer.h"

#include "descriptor_pool.h"
#include "framebuffer.h"
#include "gpu_buffer.h"
#include "image.h"
#include "pipeline.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "utils.h"

#include <algorithm>
#include <iostream>

using namespace rend;

CommandBuffer::CommandBuffer(VkCommandBuffer vk_command_buffer, uint32_t index)
    : _vk_command_buffer(vk_command_buffer), _index(index)
{
//    std::cout << "Constructing command buffer" << std::endl;
}

CommandBuffer::~CommandBuffer(void)
{
//    std::cout << "Destructing command buffer" << std::endl;
}

VkCommandBuffer CommandBuffer::get_handle(void) const
{
    return _vk_command_buffer;
}

void CommandBuffer::begin(void)
{
    VkCommandBufferBeginInfo info =
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .pInheritanceInfo = nullptr
    };

    VULKAN_DEATH_CHECK(vkBeginCommandBuffer(_vk_command_buffer, &info), "Failed to begin command buffer");
}

void CommandBuffer::end(void)
{
    vkEndCommandBuffer(_vk_command_buffer);
}

void CommandBuffer::reset(void)
{
    VULKAN_DEATH_CHECK(vkResetCommandBuffer(_vk_command_buffer, 0), "Failed to reset command buffer");
}

void CommandBuffer::begin_render_pass(const RenderPass& render_pass, Framebuffer* framebuffer, VkRect2D render_area, const std::vector<VkClearValue>& clear_values)
{
    VkRenderPassBeginInfo info =
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = render_pass.get_handle(),
        .framebuffer = framebuffer->get_handle(),
        .renderArea = render_area,
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data()
    };

    vkCmdBeginRenderPass(_vk_command_buffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::end_render_pass(void)
{
    vkCmdEndRenderPass(_vk_command_buffer);
}

void CommandBuffer::set_viewport(const VkViewport& viewport)
{
    vkCmdSetViewport(_vk_command_buffer, 0, 1, &viewport);
}

void CommandBuffer::set_scissors(const VkRect2D& scissor)
{
    vkCmdSetScissor(_vk_command_buffer, 0, 1, &scissor);
}

void CommandBuffer::bind_pipeline(VkPipelineBindPoint bind_point, const Pipeline& pipeline)
{
    vkCmdBindPipeline(_vk_command_buffer, bind_point, pipeline.get_handle());
}

void CommandBuffer::bind_descriptor_sets(VkPipelineBindPoint bind_point, const PipelineLayout& layout, const std::vector<DescriptorSet*>& sets)
{
    std::vector<VkDescriptorSet> vk_sets;
    vk_sets.reserve(sets.size());

    std::for_each(sets.begin(), sets.end(), [&vk_sets](DescriptorSet* s){ vk_sets.push_back(s->get_handle()); });

    vkCmdBindDescriptorSets(_vk_command_buffer, bind_point, layout.get_handle(), 0, static_cast<uint32_t>(vk_sets.size()), vk_sets.data(), 0, nullptr);
}

void CommandBuffer::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
    vkCmdDrawIndexed(_vk_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void CommandBuffer::bind_index_buffer(GPUBuffer* buffer, VkDeviceSize offset, VkIndexType index_type)
{
    vkCmdBindIndexBuffer(_vk_command_buffer, buffer->get_handle(), offset, index_type);
}

void CommandBuffer::bind_vertex_buffers(uint32_t first_binding, const std::vector<GPUBuffer*>& buffers, const std::vector<VkDeviceSize>& offsets)
{
    std::vector<VkBuffer> vk_buffers;
    vk_buffers.reserve(buffers.size());

    std::for_each(buffers.begin(), buffers.end(), [&vk_buffers](GPUBuffer* b){ vk_buffers.push_back(b->get_handle()); });

    vkCmdBindVertexBuffers(_vk_command_buffer, first_binding, static_cast<uint32_t>(buffers.size()), vk_buffers.data(), offsets.data());
}

void CommandBuffer::push_constant(const PipelineLayout& layout, VkShaderStageFlags shader_stages, uint32_t offset, uint32_t size, const void* data)
{
    vkCmdPushConstants(_vk_command_buffer, layout.get_handle(), shader_stages, offset, size, data);
}
void CommandBuffer::copy_buffer_to_image(GPUBuffer* buffer, Image* image)
{
    VkBufferImageCopy copy =
    {
        .bufferOffset           = 0,
        .bufferRowLength        = image->get_extent().width,
        .bufferImageHeight      = image->get_extent().height,
        .imageSubresource       =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = image->get_array_layers()
            },
        .imageOffset            = { 0, 0, 0 },
        .imageExtent            = image->get_extent()
    };

    vkCmdCopyBufferToImage(_vk_command_buffer, buffer->get_handle(), image->get_handle(), image->get_layout(), 1, &copy);
}

void CommandBuffer::copy_buffer_to_buffer(GPUBuffer* src, GPUBuffer* dst)
{
    VkBufferCopy copy =
    {
        .srcOffset = 0,
        .dstOffset = 0,
        .size      = src->get_size()
    };

    vkCmdCopyBuffer(_vk_command_buffer, src->get_handle(), dst->get_handle(), 1, &copy);
}

void CommandBuffer::pipeline_barrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkDependencyFlags dependency, const std::vector<VkMemoryBarrier>& memory_barriers, const std::vector<VkBufferMemoryBarrier>& buffer_memory_barriers, const std::vector<VkImageMemoryBarrier>& image_memory_barriers)
{
    vkCmdPipelineBarrier(_vk_command_buffer, src, dst, dependency, static_cast<uint32_t>(memory_barriers.size()), memory_barriers.data(), static_cast<uint32_t>(buffer_memory_barriers.size()), buffer_memory_barriers.data(), static_cast<uint32_t>(image_memory_barriers.size()), image_memory_barriers.data());
}
