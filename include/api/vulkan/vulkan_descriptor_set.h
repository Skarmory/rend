#ifndef REND_API_VULKAN_VULKAN_DESCRIPTOR_SET_H
#define REND_API_VULKAN_VULKAN_DESCRIPTOR_SET_H


#include "api/vulkan/vulkan_descriptor_set_info.h"
#include "core/descriptor_set.h"

#include <vulkan.h>

namespace rend
{

class DescriptorSetLayout;

class VulkanDescriptorSet : public DescriptorSet
{
public:
    VulkanDescriptorSet(const std::string& name, const DescriptorSetLayout& layout, const VulkanDescriptorSetInfo& vk_set_info);
    ~VulkanDescriptorSet(void) = default;

    const VulkanDescriptorSetInfo& vk_set_info(void) const;

    void write_bindings(void) const override;

private:
    VulkanDescriptorSetInfo _vk_set_info{};
};

}

#endif
