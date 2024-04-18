#include "api/vulkan/vulkan_descriptor_set_layout.h"

using namespace rend;

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const std::string& name, RendHandle rend_handle, VkDescriptorSetLayout vk_handle)
    :
        DescriptorSetLayout(name, rend_handle),
        _vk_handle(vk_handle)
{
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::vk_handle(void) const
{
    return _vk_handle;
}

