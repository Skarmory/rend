#ifndef REND_DESCRIPTOR_SET_LAYOUT_H
#define REND_DESCRIPTOR_SET_LAYOUT_H

#include <vulkan.h>
#include <vector>

#include "rend_defs.h"

namespace rend
{

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(void) = default;
    ~DescriptorSetLayout(void);

    DescriptorSetLayout(const DescriptorSetLayout&)            = delete;
    DescriptorSetLayout(DescriptorSetLayout&&)                 = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&)      = delete;

    StatusCode create_descriptor_set_layout(void);

    void add_uniform_buffer_binding(uint32_t slot, ShaderType shader_stage);
    void add_combined_image_sampler_binding(uint32_t slot, ShaderType shader_stage);

    VkDescriptorSetLayout                            get_handle(void) const;
    const std::vector<VkDescriptorSetLayoutBinding>& get_layout_bindings(void) const;

private:
    std::vector<VkDescriptorSetLayoutBinding> _bindings;

    VkDescriptorSetLayout _vk_layout { VK_NULL_HANDLE };
};

}

#endif

