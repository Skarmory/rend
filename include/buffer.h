#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.h>

namespace rend
{

class LogicalDevice;

class Buffer
{
    friend class LogicalDevice;

public:
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&)      = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&)      = delete;

    VkBuffer           get_handle(void) const;
    VkBufferUsageFlags get_usage(void) const;
    size_t             get_size(void) const;

    bool load(void* data, size_t size_bytes);

private:
    Buffer(LogicalDevice* device, size_t capacity, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage);
    ~Buffer(void);

private:
    VkBuffer           _vk_buffer;
    VkDeviceMemory     _vk_memory;
    VkBufferUsageFlags _vk_buffer_usage;

    LogicalDevice* _device;
    size_t         _capacity;
};

}

#endif
