#ifndef REND_MEM_BLOCK_H
#define REND_MEM_BLOCK_H

#include "data_array.h"
#include "rend_defs.h"

#include <memory>
#include <vulkan.h>

namespace rend::vkal::memory
{

class MemAlloc;
class MemAllocStrategy;
class MemHeap;

typedef rend::core::DataAccessor<MemAlloc> MemAllocAccessor;

/**
 * Holds info about a block of GPU memory.
 * Creates MemAlloc instances from the GPU memory in a pooled manner.
 * MemAllocs will be of size "alloc_size", parameterised on MemBlock initialisation
 */
class MemBlock
{
    friend class MemHeap;

    struct __MemBlock;
    std::unique_ptr<__MemBlock> _;

public:
    MemBlock(void); 
    ~MemBlock(void);

    MemBlock(const MemBlock&)            = delete;
    MemBlock& operator=(const MemBlock&) = delete;

    MemBlock(MemBlock&&);
    MemBlock& operator=(MemBlock&&);

    StatusCode create(size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage);

    // Accessors
    size_t           capacity(void) const;
    bool             compatible(const VkMemoryRequirements& memory_reqs, VkMemoryPropertyFlags memory_props, const VkMemoryType& memory_type, ResourceUsage resource_usage);

    // Mutators
    MemAllocAccessor create_mem_alloc(uint32_t offset, size_t size_bytes);
    bool             write(void* data, size_t size_bytes, uint32_t offset);
    bool             write(void* data, size_t size_bytes, uint32_t offset, void* resource);
};

}

#endif
