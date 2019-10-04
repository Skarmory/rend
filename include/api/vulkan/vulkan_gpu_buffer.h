#ifdef USE_VULKAN
#ifndef REND_VULKAN_GPU_BUFFER_H
#define REND_VULKAN_GPU_BUFFER_H

#include "gpu_buffer_base.h"
#include "rend_defs.h"

#include <vulkan.h>

namespace rend
{

class DeviceContext;

class VulkanGPUBuffer : public GPUBufferBase
{
public:
    VulkanGPUBuffer(DeviceContext& context);
    ~VulkanGPUBuffer(void);
    VulkanGPUBuffer(const VulkanGPUBuffer&) = delete;
    VulkanGPUBuffer(VulkanGPUBuffer&&)      = delete;
    VulkanGPUBuffer& operator=(const VulkanGPUBuffer&) = delete;
    VulkanGPUBuffer& operator=(VulkanGPUBuffer&&)      = delete;

    VkBuffer              get_handle(void) const;
    VkBufferUsageFlags    get_usage(void) const;

    VkDeviceMemory        get_memory(void) const;
    VkMemoryPropertyFlags get_memory_properties(void) const;

protected:
    StatusCode create_buffer(size_t size_bytes, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage);

private:
    DeviceContext&        _context;

    VkBuffer              _vk_buffer;
    VkBufferUsageFlags    _vk_buffer_usage;
    VkDeviceMemory        _vk_memory;
    VkMemoryPropertyFlags _vk_memory_properties;
};

}

#endif
#endif
