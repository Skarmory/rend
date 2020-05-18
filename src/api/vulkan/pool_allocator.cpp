#include "pool_allocator.h"

#include <assert.h>

#include "device_context.h"
#include "gpu_memory_interface.h"
#include "mem_block.h"

using namespace rend;
using namespace rend::vkal::memory;

PoolAllocator::PoolAllocator(size_t capacity, const VkMemoryRequirements& memory_requirements, VkMemoryPropertyFlags memory_property_flags, ResourceUsage usage, size_t alloc_size)
    : MemAllocatorBase(capacity, memory_requirements, memory_property_flags, usage)
    , _max_allocs(capacity / alloc_size)
    , _alloc_size(alloc_size)
{
    uint32_t offset{ 0 };
    for (size_t i{ 0 }; i < _max_allocs; ++i)
    {
        rend::core::DataArrayHandle handle = _allocs.allocate(*this, offset, alloc_size);
        _free_handles.push_back(handle);
    }
}

MemHandle PoolAllocator::allocate([[maybe_unused]] size_t bytes)
{
    assert((_used <= _max_allocs) && "Pool allocator is out of free allocs!");

    ++_used;

    MemHandle handle = _free_handles.back();
    _free_handles.pop_back();

    return handle;
}

void PoolAllocator::deallocate(MemHandle handle)
{
    if (!_allocs.check_valid(handle))
    {
        assert(true && "Pool allocator attempt to deallocate invalid MemHandle");
        return;
    }

    _free_handles.push_back(handle);
}
