#ifndef DESCRIPTOR_SET_LAYOUT_H
#define DESCRIPTOR_SET_LAYOUT_H

#include <vulkan/vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;

class DescriptorSetLayout
{
    friend class LogicalDevice;

public:
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&)      = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&)      = delete;

    VkDescriptorSetLayout get_handle(void) const;

private:
    DescriptorSetLayout(LogicalDevice* device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    ~DescriptorSetLayout(void);

private:
    VkDescriptorSetLayout _vk_layout;

    LogicalDevice* _device;
};

}

#endif

