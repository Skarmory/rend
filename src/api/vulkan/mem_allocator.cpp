//#include "mem_allocator.h"
//
//#include "device_context.h"
//#include "gpu_memory_interface.h"
//#include "mem_block.h"
//#include "vulkan_device_context.h"
//
//using namespace rend;
//using namespace rend::vkal;
//using namespace rend::vkal::memory;
//
//MemAllocatorBase::MemAllocatorBase(size_t capacity, const VkMemoryRequirements& memory_requirements, VkMemoryPropertyFlags memory_property_flags, rend::ResourceUsage usage)
//{
//	auto& memory_interface = *static_cast<VulkanDeviceContext&>(DeviceContext::instance()).memory_interface();
//	_block = memory_interface.create_block(capacity, memory_requirements, memory_property_flags, usage);
//}
//
//MemAllocatorBase::~MemAllocatorBase(void)
//{
//	_block.release();
//}
