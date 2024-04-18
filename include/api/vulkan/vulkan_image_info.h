#ifndef REND_API_VULKAN_VULKAN_IMAGE_INFO_H
#define REND_API_VULKAN_VULKAN_IMAGE_INFO_H

#include <vulkan.h>

namespace rend
{

struct VulkanImageInfo
{
    VkImage        image{ VK_NULL_HANDLE };
    VkDeviceMemory memory{ VK_NULL_HANDLE };
    VkImageView    view{ VK_NULL_HANDLE };
    VkSampler      sampler{ VK_NULL_HANDLE };
    bool           is_swapchain{ false };
};

}

#endif
