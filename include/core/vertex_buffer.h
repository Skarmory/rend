#ifndef REND_VERTEX_BUFFER_H
#define REND_VERETX_BUFFER_H

#include "rend_defs.h"

#include <cstdint>
#include <cstddef>

namespace rend
{

class VertexBuffer
{
public:
    VertexBuffer(void) = default;
    ~VertexBuffer(void) = default;

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = delete;
    const VertexBuffer& operator=(const VertexBuffer&) = delete;
    const VertexBuffer& operator=(VertexBuffer&&) = delete;

    VertexBufferHandle get_handle() const;

    bool create_vertex_buffer(uint32_t vertices_count, size_t vertex_size);

private:
    VertexBufferHandle _handle{ rend::NULL_HANDLE };
};

}

#endif
