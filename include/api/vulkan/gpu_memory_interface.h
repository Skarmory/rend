//#ifndef REND_GPU_MEMORY_INTERFACE_H
//#define REND_GPU_MEMORY_INTERFACE_H
//
//#include "data_array.h"
//#include "mem_heap.h"
//#include "rend_defs.h"
//
//#include <vector>
//#include <vulkan.h>
//
//namespace rend
//{
//class PhysicalDevice;
//}
//
//namespace rend::vkal::memory
//{
//
//class MemBlock;
//
///*
// * Class that contains the memory heaps and properties of the GPU.
// */
//class GPUMemoryInterface 
//{
//public:
//    GPUMemoryInterface(void)  = default;
//    ~GPUMemoryInterface(void) = default;
//
//    GPUMemoryInterface(const GPUMemoryInterface&)            = delete;
//    GPUMemoryInterface(GPUMemoryInterface&&)                 = delete;
//    GPUMemoryInterface& operator=(const GPUMemoryInterface&) = delete;
//    GPUMemoryInterface& operator=(GPUMemoryInterface&&)      = delete;
//
//    StatusCode     create(const PhysicalDevice& gpu);
//    MemBlockHandle create_block(size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, ResourceUsage resource_usage);
//
//private:
//    VkMemoryType _find_memory_type(uint32_t memory_types, VkMemoryPropertyFlags props);
//
//private:
//    std::vector<MemHeap>  _heaps;
//    const PhysicalDevice* _gpu{ nullptr };
//};
//
//}
//
//#endif
