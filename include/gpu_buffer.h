#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include <vulkan/vulkan.h>

namespace rend
{

class LogicalDevice;

class GPUBuffer
{
    friend class LogicalDevice;

public:
    GPUBuffer(const GPUBuffer&) = delete;
    GPUBuffer(GPUBuffer&&)      = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer& operator=(GPUBuffer&&)      = delete;

    VkBuffer           get_handle(void) const;
    VkBufferUsageFlags get_usage(void) const;
    size_t             get_size(void) const;

    VkDeviceMemory        get_memory(void) const;
    VkMemoryPropertyFlags get_memory_properties(void) const;

    //bool load(void* data, size_t size_bytes);

private:
    GPUBuffer(LogicalDevice* device, size_t capacity, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage);
    ~GPUBuffer(void);

private:
    VkBuffer              _vk_buffer;
    VkDeviceMemory        _vk_memory;
    VkMemoryPropertyFlags _vk_memory_properties;
    VkBufferUsageFlags    _vk_buffer_usage;

    LogicalDevice* _device;
    size_t         _capacity;
};

}

#endif
