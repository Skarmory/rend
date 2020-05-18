#ifndef REND_MEM_ALLOCATOR_H
#define REND_MEM_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <vulkan.h>

#include "rend_defs.h"
#include "data_array.h"
#include "mem_alloc.h"

namespace rend::vkal::memory
{

class MemAlloc;
class MemBlock;

typedef uint32_t MemHandle;
typedef rend::core::DataAccessor<MemBlock> MemBlockHandle;

/*
 * Base class for memory allocators.
 */
class MemAllocatorBase
{
public:
    MemAllocatorBase(size_t capacity, const VkMemoryRequirements& memory_requirements, VkMemoryPropertyFlags memory_property_flags, rend::ResourceUsage usage);
    virtual ~MemAllocatorBase(void);

    virtual MemHandle allocate(size_t size_bytes) = 0;
    virtual void      deallocate(MemHandle handle) = 0;
    MemAlloc*         get(MemHandle handle) { return _allocs.get(handle); }

protected:
    MemBlockHandle                  _block;
    rend::core::DataArray<MemAlloc> _allocs;
};

}

#endif
