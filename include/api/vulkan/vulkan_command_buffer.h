#ifndef REND_API_VULKAN_VULKAN_COMMAND_BUFFER_H
#define REND_API_VULKAN_VULKAN_COMMAND_BUFFER_H

#include "core/command_buffer.h"

#include <vector>
#include <vulkan.h>

namespace rend
{

class DescriptorSet;
class Framebuffer;
class GPUBuffer;
class GPUTexture;
struct PerPassData;
class Pipeline;
class PipelineLayout;
class RenderPass;

class VulkanCommandBuffer : public CommandBuffer
{

public:
    VulkanCommandBuffer(const std::string& name, VkCommandBuffer vk_handle);
    ~VulkanCommandBuffer(void) = default;

    // Common functions
    CommandBufferState get_state(void) const override;
    void reset(void) override;
    void bind_descriptor_sets(PipelineBindPoint bind_point, const PipelineLayout& pipeline_layout, const std::vector<const DescriptorSet*> descriptor_sets) override;
    void bind_pipeline(PipelineBindPoint bind_point, const Pipeline& pipeline) override;
    void bind_vertex_buffer(const GPUBuffer& vertex_buffer) override;
    void bind_index_buffer(const GPUBuffer& index_buffer) override;
    void blit(const GPUTexture& src, const GPUTexture& dst) override;
    void copy(const GPUBuffer& src, const GPUBuffer& dst, const BufferBufferCopyInfo& info) override;
    void copy(const GPUBuffer& src, const GPUTexture& dst, const BufferImageCopyInfo& info) override;
    void copy(const GPUTexture& src, const GPUTexture& dst, const ImageImageCopyInfo& info) override;
    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) override;
    void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) override;
    void push_constant(const PipelineLayout& layout, ShaderStages stages, uint32_t offset, size_t size, const void* data) override;
    void set_viewport(const std::vector<ViewportInfo>& viewports) override;
    void set_scissor(const std::vector<ViewportInfo>& scissors) override;

    // Vulkan-specific
    VkCommandBuffer vk_handle(void) const;
    void transition_image(GPUTexture& texture, PipelineStages src_stages, PipelineStages dst_stages, ImageLayout new_layout);
    void transition_image(GPUTexture& texture, ImageLayout layout, uint32_t mips, uint32_t layers, PipelineStages src_stages, PipelineStages dst_stages, ImageLayout new_layout);
    bool begin(void);
    void end(void);
    void begin_render_pass(const RenderPass& render_pass, const PerPassData& per_pass_data);
    void end_render_pass(void);
    void next_subpass(void);
    void pipeline_barrier(const PipelineBarrierInfo& info);

private:
    VkCommandBuffer _vk_handle{ VK_NULL_HANDLE };
    CommandBufferState _state{ CommandBufferState::INITIAL };
};

}

#endif
