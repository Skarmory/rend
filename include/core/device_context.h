#pragma once

#ifndef REND_DEVICE_CONTEXT_H
#define REND_DEVICE_CONTEXT_H

#include "rend_defs.h"
#include "resource.h"

#include <cstddef>

namespace rend
{

class DescriptorSet;

class DeviceContext
{
public:
    static DeviceContext& instance(void);

    virtual StatusCode create(void) = 0;
    virtual void       destroy(void) = 0;

    [[nodiscard]] virtual VertexBufferHandle        create_vertex_buffer(uint32_t vertices_count, size_t vertex_size) = 0;
    [[nodiscard]] virtual IndexBufferHandle         create_index_buffer(uint32_t indices_count, size_t index_size) = 0;
    [[nodiscard]] virtual UniformBufferHandle       create_uniform_buffer(size_t bytes) = 0;
    [[nodiscard]] virtual TextureHandle             create_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mips, uint32_t layers, Format format, MSAASamples samples, ImageUsage usage) = 0;
    [[nodiscard]] virtual ShaderHandle              create_shader(const ShaderStage type, const void* code, const size_t bytes) = 0;
    [[nodiscard]] virtual FramebufferHandle         create_framebuffer(const FramebufferInfo& info) = 0;
    [[nodiscard]] virtual RenderPassHandle          create_render_pass(const RenderPassInfo& info) = 0;
    [[nodiscard]] virtual PipelineLayoutHandle      create_pipeline_layout(const PipelineLayoutInfo& info) = 0;
    [[nodiscard]] virtual PipelineHandle            create_pipeline(const PipelineInfo& info) = 0;
    [[nodiscard]] virtual CommandPoolHandle         create_command_pool(void) = 0;
    [[nodiscard]] virtual CommandBufferHandle       create_command_buffer(CommandPoolHandle pool_handle) = 0;
    [[nodiscard]] virtual DescriptorPoolHandle      create_descriptor_pool(const DescriptorPoolInfo& info) = 0;
    [[nodiscard]] virtual DescriptorSetLayoutHandle create_descriptor_set_layout(const DescriptorSetLayoutInfo& info) = 0;
    [[nodiscard]] virtual DescriptorSetHandle       create_descriptor_set(const DescriptorSetInfo& info) = 0;

    virtual void destroy_buffer(BufferHandle handle) = 0;
    virtual void destroy_texture(Texture2DHandle handle ) = 0;
    virtual void destroy_shader(ShaderHandle handle) = 0;
    virtual void destroy_framebuffer(FramebufferHandle handle) = 0;
    virtual void destroy_render_pass(RenderPassHandle handle) = 0;
    virtual void destroy_pipeline_layout(PipelineLayoutHandle handle) = 0;
    virtual void destroy_pipeline(PipelineHandle handle) = 0;
    virtual void destroy_command_buffer(CommandBufferHandle handle) = 0;
    virtual void destroy_descriptor_pool(DescriptorPoolHandle handle) = 0;
    virtual void destroy_descriptor_set_layout(DescriptorSetLayoutHandle handle) = 0;
    virtual void destroy_descriptor_set(DescriptorSetHandle handle) = 0;

    // Command Buffer functions
    virtual void bind_descriptor_sets(CommandBufferHandle cmd_buffer, PipelineBindPoint bind_point, PipelineHandle pipeline_handle, DescriptorSet* descriptor_set, uint32_t descriptor_set_count) = 0;
    virtual void bind_pipeline(CommandBufferHandle cmd_buffer, PipelineBindPoint bind_point, PipelineHandle handle) = 0;
    virtual void bind_vertex_buffer(CommandBufferHandle command_buffer_handle, BufferHandle handle) = 0;
    virtual void bind_index_buffer(CommandBufferHandle command_buffer_handle, BufferHandle handle) = 0;
    virtual void command_buffer_begin(CommandBufferHandle command_buffer_handle) = 0;
    virtual void command_buffer_end(CommandBufferHandle command_buffer_handle) = 0;
    virtual void command_buffer_reset(CommandBufferHandle command_buffer_handle) = 0;
    virtual void copy_buffer_to_buffer(CommandBufferHandle command_buffer_handle, BufferHandle src_buffer_handle, BufferHandle dst_buffer_handle, const BufferBufferCopyInfo& info) = 0;
    virtual void copy_buffer_to_image(CommandBufferHandle command_buffer_handle, BufferHandle src_buffer_handle, TextureHandle dst_texture_handle, const BufferImageCopyInfo& info) = 0;
    virtual void draw(CommandBufferHandle command_buffer_handle, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) = 0;
    virtual void draw_indexed(CommandBufferHandle command_buffer_handle, uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance) = 0;
    virtual void pipeline_barrier(const CommandBufferHandle command_buffer_handle, const PipelineBarrierInfo& info) = 0;
    virtual void push_constant(const CommandBufferHandle command_buffer_handle, const PipelineLayoutHandle layout_handle, ShaderStages stages, uint32_t offset, uint32_t size, const void* data) = 0;
    virtual void set_viewport(const CommandBufferHandle command_buffer_handle, const ViewportInfo* infos, uint32_t infos_count) = 0;
    virtual void set_scissor(const CommandBufferHandle command_buffer_handle, const ViewportInfo* infos, uint32_t infos_count) = 0;
    virtual void begin_render_pass(const CommandBufferHandle command_buffer_handle, const RenderPassHandle render_pass_handle, const FramebufferHandle framebuffer_handle, const RenderArea render_area, const ColourClear clear_colour, const DepthStencilClear clear_depth_stencil ) = 0;
    virtual void end_render_pass(const CommandBufferHandle command_buffer_handle) = 0;

    virtual void add_descriptor_binding(const DescriptorSetHandle handle, const DescriptorSetBinding& binding) = 0;

protected:
    static DeviceContext* _service;
};

}

#endif
