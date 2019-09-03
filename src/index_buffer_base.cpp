#include "index_buffer_base.h"

using namespace rend;

IndexBufferBase::IndexBufferBase(void)
    : _count(static_cast<uint32_t>(0)), _bytes(static_cast<size_t>(0))
{
}

IndexBufferBase::~IndexBufferBase(void)
{
}

uint32_t IndexBufferBase::count(void) const
{
    return _count;
}

size_t IndexBufferBase::bytes(void) const
{
    return _bytes;
}
