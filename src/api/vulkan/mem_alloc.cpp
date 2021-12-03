#include "api/vulkan/mem_alloc.h"

#include "api/vulkan/mem_allocator.h"

using namespace rend;
using namespace rend::vkal;
using namespace rend::vkal::memory;

MemAlloc::MemAlloc(MemAllocatorBase& owner, uint32_t offset, size_t size_bytes)
    : _offset(offset)
    , _size_bytes(size_bytes)
    , _owner(&owner)
{
}

MemAlloc::~MemAlloc(void)
{
    // TODO: Add stuff
}
