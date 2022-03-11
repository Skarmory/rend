#ifndef REND_COMMAND_BUFFER_H
#define REND_COMMAND_BUFFER_H

#include "rend_defs.h"

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
public:
    CommandBuffer(void)                            = default;
    ~CommandBuffer(void)                           = default;
    CommandBuffer(const CommandBuffer&)            = delete;
    CommandBuffer(CommandBuffer&&)                 = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&)      = delete;

    bool create(CommandPoolHandle pool_handle);
    void destroy(void);
    CommandBufferHandle handle(void) const;
    bool recorded(void) const;

    void bind_descriptor_sets(PipelineBindPoint bind_point, const PipelineLayout& pipeline_layout, DescriptorSet* descriptor_sets, size_t descriptor_sets_count);
    void bind_pipeline(PipelineBindPoint bind_point, const Pipeline& pipeline);
    void bind_vertex_buffer(const GPUBuffer& vertex_buffer);
    void bind_index_buffer(const GPUBuffer& index_buffer);
    void copy(const GPUBuffer& src, const GPUBuffer& dst);
    void copy(const GPUBuffer& src, const GPUTexture& dst);
    void transition_image(GPUTexture& texture, PipelineStages src_stages, PipelineStages dst_stages, ImageLayout new_layout);
    void reset(void);

private:
    CommandBufferHandle _handle{ NULL_HANDLE };
    bool _recorded{ false };
};

}

#endif
