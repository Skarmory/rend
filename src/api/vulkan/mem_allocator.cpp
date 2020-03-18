#include "mem_allocator.h"

#include "mem_block.h"

#include <cassert>

using namespace rend;
using namespace rend::vkal;
using namespace rend::vkal::memory;

/* ----- MemHandle ----- */

MemHandle::MemHandle(MemAlloc& alloc, uint32_t index)
    : _index(index)
    , _allocation(&alloc)
{
}

MemAlloc* MemHandle::operator->(void)
{
    return _allocation;
}

uint32_t MemHandle::generation(void) const
{
    return _generation;
}

uint32_t MemHandle::index(void) const
{
    return _index;
}

/* ----- MemAllocatorBase ----- */

MemAllocatorBase::MemAllocatorBase(MemBlock& block)
    : _block(&block)
{
}

MemBlock* MemAllocatorBase::block(void) const
{
    return _block;
}

/* ----- PoolAllocator ----- */

PoolAllocator::PoolAllocator(MemBlock& block, size_t alloc_size)
    : MemAllocatorBase(block)
    , _alloc_size(alloc_size)
{
    size_t allocs_count = block.capacity() / alloc_size;

    _handles.reserve(allocs_count);
    _free_list.reserve(allocs_count);

    uint32_t offset = 0;

    // Create mem allocs and a free list of indices
    for(uint32_t idx = 0; idx < allocs_count; ++idx)
    {
        _handles.push_back(MemHandle(*block.create_mem_alloc(offset, alloc_size), idx));
        _free_list.push_back(idx);
        offset += alloc_size;
    }
}

MemHandle PoolAllocator::allocate(size_t bytes)
{
    assert(!_free_list.empty() && "Pool allocator is out of free allocs!");

    uint32_t idx = _free_list.back();
    _free_list.pop_back();

    return _handles[idx];
}

void PoolAllocator::deallocate(MemHandle handle)
{
    _free_list.push_back(handle._index);
    ++_handles[handle._index]._generation;
}
