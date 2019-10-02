#include "pipeline_layout.h"

#include "device_context.h"
#include "logical_device.h"
#include "descriptor_set_layout.h"

using namespace rend;

PipelineLayout::PipelineLayout(DeviceContext* context)
    : _context(context),
      _vk_layout(VK_NULL_HANDLE)
{
}

PipelineLayout::~PipelineLayout(void)
{
    vkDestroyPipelineLayout(_context->get_device()->get_handle(), _vk_layout, nullptr);
}

bool PipelineLayout::create_pipeline_layout(const std::vector<DescriptorSetLayout*>& desc_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges)
{
    if(_vk_layout != VK_NULL_HANDLE)
        return false;

    std::vector<VkDescriptorSetLayout> vk_layouts;
    vk_layouts.reserve(desc_set_layouts.size());

    for(DescriptorSetLayout* layout : desc_set_layouts)
        vk_layouts.push_back(layout->get_handle());

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

    if(vkCreatePipelineLayout(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_layout) != VK_SUCCESS)
        return false;

    return true;
}

VkPipelineLayout PipelineLayout::get_handle(void) const
{
    return _vk_layout;
}
