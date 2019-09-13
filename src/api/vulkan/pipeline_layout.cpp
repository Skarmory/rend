#include "pipeline_layout.h"

#include "logical_device.h"
#include "descriptor_set_layout.h"
#include "utils.h"

#include <algorithm>

using namespace rend;

PipelineLayout::PipelineLayout(LogicalDevice* device, const std::vector<DescriptorSetLayout*>& desc_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges) : _device(device)
{
    std::vector<VkDescriptorSetLayout> vk_layouts;
    vk_layouts.reserve(desc_set_layouts.size());

    std::for_each(desc_set_layouts.begin(), desc_set_layouts.end(), [&vk_layouts](DescriptorSetLayout* l){ vk_layouts.push_back(l->get_handle()); });

    VkPipelineLayoutCreateInfo create_info =
    {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = static_cast<uint32_t>(vk_layouts.size()),
        .pSetLayouts            = vk_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges.size()),
        .pPushConstantRanges    = push_constant_ranges.data()
    };

    VULKAN_DEATH_CHECK(vkCreatePipelineLayout(_device->get_handle(), &create_info, nullptr, &_vk_layout), "Failed to create pipeline layout");
}

PipelineLayout::~PipelineLayout(void)
{
    vkDestroyPipelineLayout(_device->get_handle(), _vk_layout, nullptr);
}

VkPipelineLayout PipelineLayout::get_handle(void) const
{
    return _vk_layout;
}