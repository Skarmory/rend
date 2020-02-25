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
    VulkanGPUBuffer(void) = default;
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
    VkBuffer              _vk_buffer { VK_NULL_HANDLE };
    VkBufferUsageFlags    _vk_buffer_usage { 0 };
    VkDeviceMemory        _vk_memory { VK_NULL_HANDLE };
    VkMemoryPropertyFlags _vk_memory_properties { 0 };
};

}

#endif
#endif
