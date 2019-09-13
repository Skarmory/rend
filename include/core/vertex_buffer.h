#ifndef REND_VERTEX_BUFFER_H
#define REND_VERETX_BUFFER_H

#include <cstdint>
#include <cstddef>

#ifdef USE_VULKAN
#include "vulkan_vertex_buffer.h"
#endif

namespace rend
{

#ifdef USE_VULKAN
class VertexBuffer : public VulkanVertexBuffer
#elif
class VertexBuffer
#endif
{
public:
    VertexBuffer(DeviceContext* context);
    ~VertexBuffer(void);

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = delete;
    const VertexBuffer& operator=(const VertexBuffer&) = delete;
    const VertexBuffer& operator=(VertexBuffer&&) = delete;

    bool create_vertex_buffer(uint32_t vertices_count, size_t vertex_size);
};

}

#endif
