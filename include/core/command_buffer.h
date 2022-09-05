#ifndef REND_COMMAND_BUFFER_H
#define REND_COMMAND_BUFFER_H

#include "core/alloc/allocator.h"
#include "core/rend_defs.h"

#include <vector>

namespace rend
{

class DescriptorSet;
class Framebuffer;
class GPUBuffer;
class GPUTexture;
class Pipeline;
class PipelineLayout;
class RenderPass;

class CommandBuffer
{
    friend class CommandPool;
    friend class Allocator<CommandBuffer>;

public:
    CommandBuffer(const CommandBuffer&)            = delete;
    CommandBuffer(CommandBuffer&&)                 = default;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&)      = default;

    CommandBufferHandle handle(void) const;
    bool recorded(void) const;

    void bind_descriptor_sets(PipelineBindPoint bind_point, const PipelineLayout& pipeline_layout, const std::vector<DescriptorSet*> descriptor_sets);
    void bind_pipeline(PipelineBindPoint bind_point, const Pipeline& pipeline);
    void bind_vertex_buffer(const GPUBuffer& vertex_buffer);
    void bind_index_buffer(const GPUBuffer& index_buffer);
    void blit(const GPUTexture& src, const GPUTexture& dst);
    void copy(const GPUBuffer& src, const GPUBuffer& dst, const BufferBufferCopyInfo& info);
    void copy(const GPUBuffer& src, const GPUTexture& dst, const BufferImageCopyInfo& info);
    void copy(const GPUTexture& src, const GPUTexture& dst, const ImageImageCopyInfo& info);
    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
    void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
    void push_constant(const PipelineLayout& layout, ShaderStages stages, uint32_t offset, size_t size, const void* data);
    void transition_image(GPUTexture& texture, PipelineStages src_stages, PipelineStages dst_stages, ImageLayout new_layout);
    void transition_image(
        TextureHandle handle,
        ImageLayout layout,
        uint32_t mips,
        uint32_t layers,
        PipelineStages src_stages,
        PipelineStages dst_stages,
        ImageLayout new_layout);
    void reset(void);

    void set_viewport(const ViewportInfo* viewport_infos, size_t viewport_infos_count);
    void set_scissor(const ViewportInfo* scissor_infos, size_t scissor_infos_count);

    void begin(void);
    void end(void);
    void begin_render_pass(RenderPass& render_pass, Framebuffer& framebuffer, const RenderArea render_area, const ColourClear clear_colour, const DepthStencilClear clear_depth_stencil);
    void end_render_pass(void);
    void next_subpass(void);

private:
    CommandBuffer(void);
    ~CommandBuffer(void);

private:
    CommandBufferHandle _handle{ NULL_HANDLE };
    CommandBufferHandle _pool_reference_handle{ NULL_HANDLE };
    bool                _recorded{ false };
};

}

#endif
