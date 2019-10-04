#include "vertex_buffer_base.h"

using namespace rend;

VertexBufferBase::VertexBufferBase(void)
    : _count(0),
      _vertex_bytes(0)
{
}

VertexBufferBase::~VertexBufferBase(void)
{
}

uint32_t VertexBufferBase::count(void) const
{
    return _count;
}

size_t VertexBufferBase::vertex_bytes(void) const
{
    return _vertex_bytes;
}
