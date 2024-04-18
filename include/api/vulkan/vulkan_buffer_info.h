#ifndef REND_API_VULKAN_VULKAN_BUFFER_INFO_H
#define REND_API_VULKAN_VULKAN_BUFFER_INFO_H

#include <vulkan.h>

namespace rend
{

struct VulkanBufferInfo
{
    VkBuffer       buffer{ VK_NULL_HANDLE };
    VkDeviceMemory memory{ VK_NULL_HANDLE };
    size_t         bytes{ 0 };
};

}

#endif
