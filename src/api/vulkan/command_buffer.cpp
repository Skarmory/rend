#include "command_buffer.h"

#include "descriptor_pool.h"
#include "framebuffer.h"
#include "vulkan_gpu_buffer.h"
#include "image.h"
#include "pipeline.h"
#include "pipeline_layout.h"
#include "render_pass.h"

#include "vulkan_index_buffer.h"
#include "index_buffer.h"

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

void CommandBuffer::end_render_pass(void)
{
    _recorded = true;

    vkCmdEndRenderPass(_vk_command_buffer);
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

    vkCmdBindIndexBuffer(_vk_command_buffer, buffer.get_handle(), offset, index_type);
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

void CommandBuffer::push_constant(const PipelineLayout& layout, VkShaderStageFlags shader_stages, uint32_t offset, uint32_t size, const void* data)
{
    _recorded = true;

    vkCmdPushConstants(_vk_command_buffer, layout.get_handle(), shader_stages, offset, size, data);
}

void CommandBuffer::copy_buffer_to_image(const VulkanGPUBuffer& buffer, const Image& image)
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

    vkCmdCopyBufferToImage(_vk_command_buffer, buffer.get_handle(), image.get_handle(), image.get_layout(), 1, &copy);
}

void CommandBuffer::copy_buffer_to_buffer(const VulkanGPUBuffer& src, const VulkanGPUBuffer& dst)
{
    _recorded = true;

    VkBufferCopy copy =
    {
        .srcOffset = 0,
        .dstOffset = 0,
        .size      = src.bytes()
    };

    vkCmdCopyBuffer(_vk_command_buffer, src.get_handle(), dst.get_handle(), 1, &copy);
}

void CommandBuffer::pipeline_barrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkDependencyFlags dependency, const std::vector<VkMemoryBarrier>& memory_barriers, const std::vector<VkBufferMemoryBarrier>& buffer_memory_barriers, const std::vector<VkImageMemoryBarrier>& image_memory_barriers)
{
    _recorded = true;

    vkCmdPipelineBarrier(_vk_command_buffer, src, dst, dependency, static_cast<uint32_t>(memory_barriers.size()), memory_barriers.data(), static_cast<uint32_t>(buffer_memory_barriers.size()), buffer_memory_barriers.data(), static_cast<uint32_t>(image_memory_barriers.size()), image_memory_barriers.data());
}
