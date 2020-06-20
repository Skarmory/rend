#ifndef REND_VERTEX_BUFFER_H
#define REND_VERETX_BUFFER_H

#include <cstdint>
#include <cstddef>

#include "gpu_buffer_base.h"
#include "rend_defs.h"

namespace rend
{

class VertexBuffer : public GPUBufferBase
{
public:
    VertexBuffer(void) = default;
    ~VertexBuffer(void) = default;

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = delete;
    const VertexBuffer& operator=(const VertexBuffer&) = delete;
    const VertexBuffer& operator=(VertexBuffer&&) = delete;

    bool create_vertex_buffer(uint32_t vertices_count, size_t vertex_size);
};

}

#endif
