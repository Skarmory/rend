#ifndef REND_CORE_COMMAND_BUFFER_H
#define REND_CORE_COMMAND_BUFFER_H

#include "core/rend_defs.h"

#include <vector>

namespace rend
{

class DescriptorSet;
class GPUBuffer;
class GPUTexture;
class Pipeline;
class PipelineLayout;

class CommandBuffer
{
public:
    virtual ~CommandBuffer(void) = default;

    virtual bool recorded(void) const = 0;

    virtual void reset(void) = 0;
    virtual void bind_descriptor_sets(PipelineBindPoint bind_point, const PipelineLayout& pipeline_layout, const std::vector<const DescriptorSet*> descriptor_sets) = 0;
    virtual void bind_pipeline(PipelineBindPoint bind_point, const Pipeline& pipeline) = 0;
    virtual void bind_vertex_buffer(const GPUBuffer& vertex_buffer) = 0;
    virtual void bind_index_buffer(const GPUBuffer& index_buffer) = 0;
    virtual void blit(const GPUTexture& src, const GPUTexture& dst) = 0;
    virtual void copy(const GPUBuffer& src, const GPUBuffer& dst, const BufferBufferCopyInfo& info) = 0;
    virtual void copy(const GPUBuffer& src, const GPUTexture& dst, const BufferImageCopyInfo& info) = 0;
    virtual void copy(const GPUTexture& src, const GPUTexture& dst, const ImageImageCopyInfo& info) = 0;
    virtual void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) = 0;
    virtual void draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) = 0;
    virtual void push_constant(const PipelineLayout& layout, ShaderStages stages, uint32_t offset, size_t size, const void* data) = 0;
    virtual void set_viewport(const std::vector<ViewportInfo>& viewports) = 0;
    virtual void set_scissor(const std::vector<ViewportInfo>& scissors) = 0;
};

}

#endif
