#include "gpu_memory_interface.h"

#include "mem_block.h"
#include "physical_device.h"

#include <cassert>

using namespace rend;
using namespace rend::core;
using namespace rend::vkal;
using namespace rend::vkal::memory;

StatusCode GPUMemoryInterface::create(const PhysicalDevice& gpu)
{
    _gpu = &gpu;
    const VkPhysicalDeviceMemoryProperties& memory_properties = _gpu->get_memory_properties();

    _heaps.reserve(memory_properties.memoryHeapCount);

    for(uint32_t idx = 0; idx < memory_properties.memoryHeapCount; ++idx)
    {
        VkMemoryHeap heap = memory_properties.memoryHeaps[idx];
        _heaps.emplace_back(heap.size);
    }

    return StatusCode::SUCCESS;
}

MemBlockHandle GPUMemoryInterface::create_block(size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, ResourceUsage resource_usage)
{
    VkMemoryType memory_type = _find_memory_type(memory_requirements.memoryTypeBits, memory_properties);

    MemHeap* heap  = &_heaps[memory_type.heapIndex];
    //MemBlock* block = heap->find_block(memory_requirements, memory_properties, type, resource_usage);
    MemBlockHandle block = heap->create_block(block_size, memory_requirements, memory_properties, memory_type, resource_usage);

    return block;
}

VkMemoryType GPUMemoryInterface::_find_memory_type(uint32_t memory_types, VkMemoryPropertyFlags props)
{
    const VkPhysicalDeviceMemoryProperties& memory_properties = _gpu->get_memory_properties();

    // Search for a memory type that has the correct properties
    for(uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if((memory_types & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & props) == props)
        {
            // Return the heap that this memory type indexes
            return memory_properties.memoryTypes[i];
        }
    }

    assert(true || "No memory type found");
    return { 0, VK_MAX_MEMORY_TYPES };
}
