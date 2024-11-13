#include "api/vulkan/vulkan_descriptor_set.h"

#include "api/vulkan/vulkan_renderer.h"

using namespace rend;

VulkanDescriptorSet::VulkanDescriptorSet(const std::string& name, const DescriptorSetLayout& layout, const VulkanDescriptorSetInfo& vk_set_info)
    :
        DescriptorSet(name, layout),
        _vk_set_info(vk_set_info)
{
}

const VulkanDescriptorSetInfo& VulkanDescriptorSet::vk_set_info(void) const
{
    return _vk_set_info;
}

void VulkanDescriptorSet::write_bindings(void) const
{
    auto& rr = static_cast<VulkanRenderer&>(Renderer::get_instance());
    rr.write_descriptor_bindings(*this);
}
