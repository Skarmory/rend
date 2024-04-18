#include "api/vulkan/vulkan_descriptor_set.h"

using namespace rend;

VulkanDescriptorSet::VulkanDescriptorSet(const std::string& name, const DescriptorSetInfo& info, RendHandle rend_handle, const VulkanDescriptorSetInfo& vk_set_info)
    :
        DescriptorSet(name, info, rend_handle),
        _vk_set_info(vk_set_info)
{
}

const VulkanDescriptorSetInfo& VulkanDescriptorSet::vk_set_info(void) const
{
    return _vk_set_info;
}
