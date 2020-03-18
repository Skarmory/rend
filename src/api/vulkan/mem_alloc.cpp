#include "mem_alloc.h"

#include "mem_allocator.h"
#include "mem_block.h"

using namespace rend;
using namespace rend::vkal;
using namespace rend::vkal::memory;

MemAlloc::MemAlloc(uint32_t offset, size_t size_bytes)
    : _offset(offset)
    , _size_bytes(size_bytes)
{
}

MemAlloc::~MemAlloc(void)
{
    // TODO: Add stuff
}

bool MemAlloc::write(void* data, size_t size_bytes)
{
    return _allocator->block()->write(data, size_bytes, _offset);
}

bool MemAlloc::write(void* data, size_t size_bytes, void* resource)
{
    return _allocator->block()->write(data, size_bytes, _offset, resource);
}
