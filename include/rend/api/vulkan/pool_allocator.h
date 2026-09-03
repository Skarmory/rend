#ifndef REND_POOL_ALLOCATOR_H
#define REND_POOL_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rend_defs.h"
#include "mem_allocator.h"

namespace rend::vkal::memory
{

/*
 * A memory allocator that divides up a MemBlock into fixed size MemAllocs.
 */
class PoolAllocator : public MemAllocatorBase
{
public:
    PoolAllocator(size_t capacity, const VkMemoryRequirements& memory_requirements, VkMemoryPropertyFlags memory_property_flags, rend::ResourceUsage usage, size_t alloc_size);
    ~PoolAllocator(void) = default;

    [[nodiscard]] MemHandle allocate(size_t bytes) override;
    void                    deallocate(MemHandle handle) override;

private:
    size_t _max_allocs { 0 };
    size_t _used       { 0 };
    size_t _alloc_size { 0 };
    std::vector<uint32_t> _free_handles;
};

}

#endif
