#ifndef REND_COMMAND_BUFFER_H
#define REND_COMMAND_BUFFER_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandPool;
class DescriptorSet;
class Framebuffer;
class VulkanGPUBuffer;
class VulkanGPUTexture;
class IndexBuffer;
class Pipeline;
class PipelineLayout;
class RenderPass;

class CommandBuffer
{
public:
    CommandBuffer(CommandPool* pool, VkCommandBuffer vk_command_buffer);
    ~CommandBuffer(void);
    CommandBuffer(const CommandBuffer&)            = delete;
    CommandBuffer(CommandBuffer&&)                 = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&)      = delete;

    CommandPool&    get_pool(void) const;
    VkCommandBuffer get_handle(void) const;

    bool begin(void);
    bool end(void);
    bool reset(void);
    bool recording(void) const;
    bool recorded(void) const;

    void begin_render_pass(const RenderPass& render_pass, const Framebuffer& framebuffer, VkRect2D render_area, const std::vector<VkClearValue>& clear_values);
    void end_render_pass(void);

    void set_viewport(const VkViewport& viewport);
    void set_scissors(const VkRect2D& scissor);

    void bind_pipeline(VkPipelineBindPoint bind_point, const Pipeline& pipeline);
    void bind_descriptor_sets(VkPipelineBindPoint bind_point, const PipelineLayout& layout, const std::vector<DescriptorSet*>& sets);
    void bind_index_buffer(const IndexBuffer& buffer, VkDeviceSize offset, VkIndexType index_type);
    void bind_vertex_buffers(uint32_t first_binding, const std::vector<VulkanGPUBuffer*>& buffers, const std::vector<VkDeviceSize>& offsets);

    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
    void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);

    void push_constant(const PipelineLayout& layout, VkShaderStageFlags shader_stages, uint32_t offset, uint32_t size, const void* data);

    void copy_buffer_to_image(const VulkanGPUBuffer& buffer, const VulkanGPUTexture& image);
    void copy_buffer_to_buffer(const VulkanGPUBuffer& src, const VulkanGPUBuffer& dst);

    void pipeline_barrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkDependencyFlags dependency, const std::vector<VkMemoryBarrier>& memory_barriers, const std::vector<VkBufferMemoryBarrier>& buffer_memory_barriers, const std::vector<VkImageMemoryBarrier>& image_memory_barriers);

private:
    CommandPool*    _pool;
    bool            _recording;
    bool            _recorded;
    VkCommandBuffer _vk_command_buffer;
};

}

#endif
