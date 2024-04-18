#ifndef REND_API_VULKAN_VULKAN_DESCRIPTOR_SET_LAYOUT_H
#define REND_API_VULKAN_VULKAN_DESCRIPTOR_SET_LAYOUT_H

#include "core/descriptor_set_layout.h"

#include <string>
#include <vulkan.h>

namespace rend
{

class VulkanDescriptorSetLayout : public DescriptorSetLayout
{
public:
    VulkanDescriptorSetLayout(const std::string& name, RendHandle rend_handle, VkDescriptorSetLayout vk_handle);
    ~VulkanDescriptorSetLayout(void) = default;

    VkDescriptorSetLayout vk_handle(void) const;

private:
    VkDescriptorSetLayout _vk_handle{ VK_NULL_HANDLE };
};

}

#endif
