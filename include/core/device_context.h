#pragma once

#ifndef REND_DEVICE_CONTEXT_H
#define REND_DEVICE_CONTEXT_H

#include "rend_defs.h"
#include "resource.h"

#include <cstddef>

namespace rend
{

class DeviceContext
{
public:
    static DeviceContext& instance(void);

    virtual StatusCode create(void) = 0;
    virtual void       destroy(void) = 0;

    [[nodiscard]] virtual VertexBufferHandle  create_vertex_buffer(uint32_t vertices_count, size_t vertex_size) = 0;
    [[nodiscard]] virtual IndexBufferHandle   create_index_buffer(uint32_t indices_count, size_t index_size) = 0;
    [[nodiscard]] virtual UniformBufferHandle create_uniform_buffer(size_t bytes) = 0;
    [[nodiscard]] virtual TextureHandle       create_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mips, uint32_t layers, Format format, MSAASamples samples, ImageUsage usage) = 0;
    [[nodiscard]] virtual ShaderHandle        create_shader(const ShaderType type, const void* code, const size_t bytes) = 0;
    [[nodiscard]] virtual FramebufferHandle   create_framebuffer(const FramebufferInfo& info) = 0;
    [[nodiscard]] virtual RenderPassHandle    create_render_pass(const RenderPassInfo& info) = 0;

    virtual void destroy_buffer(BufferHandle handle) = 0;
    virtual void destroy_texture(Texture2DHandle handle ) = 0;
    virtual void destroy_shader(ShaderHandle handle) = 0;
    virtual void destroy_framebuffer(FramebufferHandle handle) = 0;
    virtual void destroy_render_pass(RenderPassHandle handle) = 0;

protected:
    static DeviceContext* _service;
};

}

#endif
