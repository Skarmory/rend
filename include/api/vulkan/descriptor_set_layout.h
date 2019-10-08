#ifndef REND_DESCRIPTOR_SET_LAYOUT_H
#define REND_DESCRIPTOR_SET_LAYOUT_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class DeviceContext;

class DescriptorSetLayout
{
public:
    explicit DescriptorSetLayout(DeviceContext& device);
    ~DescriptorSetLayout(void);

    DescriptorSetLayout(const DescriptorSetLayout&)            = delete;
    DescriptorSetLayout(DescriptorSetLayout&&)                 = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&)      = delete;

    bool create_descriptor_set_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

    VkDescriptorSetLayout                            get_handle(void) const;
    const std::vector<VkDescriptorSetLayoutBinding>& get_layout_bindings(void) const;

private:
    DeviceContext& _context;
    std::vector<VkDescriptorSetLayoutBinding> _bindings;

    VkDescriptorSetLayout _vk_layout;
};

}

#endif

