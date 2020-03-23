#ifndef REND_MEM_HEAP_H
#define REND_MEM_HEAP_H

#include "rend_defs.h"

#include <memory>
#include <vector>
#include <vulkan.h>

namespace rend::vkal::memory
{

class MemBlock; 
class Renderer;

/*
 * Abstraction of a Vulkan memory heap.
 * Memory heaps have different properties associated with them, and different resource usages benefit more from a specific type.
 * Creates and owns MemBlocks with the properties of the respective MemHeap.
 */
class MemHeap
{
    struct __MemHeap;
    std::unique_ptr<__MemHeap> _;

public:
    MemHeap(size_t available_bytes);
    ~MemHeap(void);

    MemHeap(const MemHeap& other) = delete;
    MemHeap& operator=(const MemHeap& other) = delete;

    MemHeap(MemHeap&& other);
    MemHeap& operator=(MemHeap&& other);

    MemBlock* create_block(size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage);
    MemBlock* find_block(const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage);
};

}

#endif
