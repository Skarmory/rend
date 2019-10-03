#include "vulkan_gpu_buffer.h"

#include "device_context.h"
#include "logical_device.h"

using namespace rend;

VulkanGPUBuffer::VulkanGPUBuffer(DeviceContext* context)
    : _context(context),
      _vk_buffer(VK_NULL_HANDLE),
      _vk_buffer_usage(0),
      _vk_memory(VK_NULL_HANDLE),
      _vk_memory_properties(0)
{
}

VulkanGPUBuffer::~VulkanGPUBuffer(void)
{
    LogicalDevice* dev = _context->get_device();
    vkFreeMemory(dev->get_handle(), _vk_memory, nullptr);
    vkDestroyBuffer(dev->get_handle(), _vk_buffer, nullptr);
}

bool VulkanGPUBuffer::create_buffer(size_t size_bytes, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage)
{
    LogicalDevice* dev = _context->get_device();
    uint32_t queue_family_index = dev->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkBufferCreateInfo create_info =
    {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .size                  = static_cast<VkDeviceSize>(size_bytes),
        .usage                 = buffer_usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices   = &queue_family_index
    };

    if(vkCreateBuffer(dev->get_handle(), &create_info, nullptr, &_vk_buffer) != VK_SUCCESS)
        return false;

    VkMemoryRequirements memory_reqs = {};
    vkGetBufferMemoryRequirements(dev->get_handle(), _vk_buffer, &memory_reqs);

    VkMemoryAllocateInfo alloc_info =
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = static_cast<VkDeviceSize>(size_bytes),
        .memoryTypeIndex = dev->find_memory_type(memory_reqs.memoryTypeBits, memory_properties)
    };

    if(vkAllocateMemory(dev->get_handle(), &alloc_info, nullptr, &_vk_memory) != VK_SUCCESS)
        return false;

    if(vkBindBufferMemory(dev->get_handle(), _vk_buffer, _vk_memory, 0) != VK_SUCCESS)
        return false;

    _bytes = size_bytes;
    _vk_memory_properties = memory_properties;
    _vk_buffer_usage = buffer_usage;

    return true;
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
