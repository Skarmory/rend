#include "api/vulkan/vulkan_descriptor_set_layout.h"

using namespace rend;

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const std::string& name, VkDescriptorSetLayout vk_handle, const DescriptorSetLayoutInfo& info)
    :
        DescriptorSetLayout(name, info),
        _vk_handle(vk_handle)
{
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::vk_handle(void) const
{
    return _vk_handle;
}

