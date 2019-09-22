#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include <vulkan.h>

namespace rend
{

class DeviceContext;

class GPUBuffer
{
public:
    GPUBuffer(DeviceContext* context);
    ~GPUBuffer(void);
    GPUBuffer(const GPUBuffer&) = delete;
    GPUBuffer(GPUBuffer&&)      = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer& operator=(GPUBuffer&&)      = delete;

    bool create(size_t capacity, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage);

    VkBuffer           get_handle(void) const;
    VkBufferUsageFlags get_usage(void) const;
    size_t             get_size(void) const;

    VkDeviceMemory        get_memory(void) const;
    VkMemoryPropertyFlags get_memory_properties(void) const;

private:
    DeviceContext*        _context;
    VkBuffer              _vk_buffer;
    VkDeviceMemory        _vk_memory;
    VkMemoryPropertyFlags _vk_memory_properties;
    VkBufferUsageFlags    _vk_buffer_usage;
    size_t                _capacity;
};

}

#endif
