#ifndef REND_API_VULKAN_VULKAN_DESCRIPTOR_SET_INFO_H
#define REND_API_VULKAN_VULKAN_DESCRIPTOR_SET_INFO_H

#include <vulkan.h>

namespace rend
{

struct VulkanDescriptorSetInfo
{
    VkDescriptorSet       set{ VK_NULL_HANDLE };
    VkDescriptorPool      pool{ VK_NULL_HANDLE };
    VkDescriptorSetLayout layout{ VK_NULL_HANDLE };
};

}

#endif
