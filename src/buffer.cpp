#include "buffer.h"

#include "logical_device.h"
#include "utils.h"

#include <cstring>

using namespace rend;

Buffer::Buffer(LogicalDevice* device, size_t capacity, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage) : _vk_buffer_usage(buffer_usage), _device(device), _capacity(capacity)
{

    uint32_t queue_family_index = _device->get_queue_family(QueueType::GRAPHICS)->get_index();
    
    VkBufferCreateInfo create_info =
    {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .size                  = static_cast<VkDeviceSize>(capacity),
        .usage           = buffer_usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices   = &queue_family_index
    };

    VULKAN_DEATH_CHECK(vkCreateBuffer(_device->get_handle(), &create_info, nullptr, &_vk_buffer), "Failed to create buffer"); 

    VkMemoryRequirements memory_reqs;
    vkGetBufferMemoryRequirements(_device->get_handle(), _vk_buffer, &memory_reqs);
    
    VkMemoryAllocateInfo alloc_info =
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = static_cast<VkDeviceSize>(capacity),
        .memoryTypeIndex = _device->find_memory_type(memory_reqs.memoryTypeBits, memory_properties)
    };

    VULKAN_DEATH_CHECK(vkAllocateMemory(_device->get_handle(), &alloc_info, nullptr, &_vk_memory), "Failed to allocate memory");

    VULKAN_DEATH_CHECK(vkBindBufferMemory(_device->get_handle(), _vk_buffer, _vk_memory, 0), "Failed to bind buffer memory");

}

Buffer::~Buffer(void)
{
    vkFreeMemory(_device->get_handle(), _vk_memory, nullptr);

    vkDestroyBuffer(_device->get_handle(), _vk_buffer, nullptr);
}

VkBuffer Buffer::get_handle(void) const
{
    return _vk_buffer;
}

VkBufferUsageFlags Buffer::get_usage(void) const
{
    return _vk_buffer_usage;
}

size_t Buffer::get_size(void) const
{
    return _capacity;
}

bool Buffer::load(void* data, size_t size_bytes)
{
    if(size_bytes > _capacity)
        return false;

    void* mapped;
    vkMapMemory(_device->get_handle(), _vk_memory, 0, static_cast<uint32_t>(size_bytes), 0, &mapped);
    memcpy(mapped, data, size_bytes);
    vkUnmapMemory(_device->get_handle(), _vk_memory);

    return true;
}
