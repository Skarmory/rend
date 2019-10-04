#ifndef REND_VERTEX_BUFFER_BASE_H
#define REND_VERTEX_BUFFER_BASE_H

#include <cstdint>
#include <cstddef>

namespace rend
{

class VertexBufferBase
{
public:
    VertexBufferBase(void);
    virtual ~VertexBufferBase(void);

    VertexBufferBase(const VertexBufferBase&) = delete;
    VertexBufferBase(VertexBufferBase&&) = delete;
    const VertexBufferBase& operator=(const VertexBufferBase&) = delete;
    const VertexBufferBase& operator=(VertexBufferBase&&) = delete;

    uint32_t count(void) const;
    size_t   vertex_bytes(void) const;

protected:
    uint32_t _count;
    size_t   _vertex_bytes;
};

}

#endif
