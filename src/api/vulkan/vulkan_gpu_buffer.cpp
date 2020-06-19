#include "vulkan_gpu_buffer.h"

#include "device_context.h"
#include "vulkan_device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

VulkanGPUBuffer::~VulkanGPUBuffer(void)
{
    auto& context = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    context.get_device()->free_memory(_vk_memory);
    context.get_device()->destroy_buffer(_vk_buffer);
}

VkBuffer VulkanGPUBuffer::get_handle(void) const
{
    return _vk_buffer;
}

VkBufferUsageFlags VulkanGPUBuffer::get_usage(void) const
{
    return _vk_buffer_usage;
}

VkDeviceMemory VulkanGPUBuffer::get_memory(void) const
{
    return _vk_memory;
}

VkMemoryPropertyFlags VulkanGPUBuffer::get_memory_properties(void) const
{
    return _vk_memory_properties;
}

StatusCode VulkanGPUBuffer::create_buffer(size_t size_bytes, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage)
{
    auto& context = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    uint32_t queue_family_index = context.get_device()->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkBufferCreateInfo create_info    = vulkan_helpers::gen_buffer_create_info();
    create_info.size                  = size_bytes;
    create_info.usage                 = buffer_usage;
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices   = &queue_family_index;

    if((_vk_buffer = context.get_device()->create_buffer(create_info)) == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    VkMemoryRequirements memory_reqs = context.get_device()->get_buffer_memory_reqs(_vk_buffer);

    VkMemoryAllocateInfo alloc_info = vulkan_helpers::gen_memory_allocate_info();
    alloc_info.allocationSize = size_bytes;
    alloc_info.memoryTypeIndex = context.get_device()->find_memory_type(memory_reqs.memoryTypeBits, memory_properties);

    _vk_memory = context.get_device()->allocate_memory(alloc_info);
    if(_vk_memory == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    if(context.get_device()->bind_buffer_memory(_vk_buffer, _vk_memory) != VK_SUCCESS)
    {
        return StatusCode::FAILURE;
    }

    _bytes = size_bytes;
    _vk_memory_properties = memory_properties;
    _vk_buffer_usage = buffer_usage;

    return StatusCode::SUCCESS;
}
