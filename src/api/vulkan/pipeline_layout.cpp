#include "pipeline_layout.h"

#include "device_context.h"
#include "logical_device.h"
#include "descriptor_set_layout.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

PipelineLayout::PipelineLayout(DeviceContext& context)
    : _context(context),
      _vk_layout(VK_NULL_HANDLE)
{
}

PipelineLayout::~PipelineLayout(void)
{
    vkDestroyPipelineLayout(_context.get_device()->get_handle(), _vk_layout, nullptr);
}

bool PipelineLayout::create_pipeline_layout(void)
{
    if(_vk_layout != VK_NULL_HANDLE)
        return false;

    std::vector<VkDescriptorSetLayout> vk_layouts;
    vk_layouts.reserve(_descriptor_set_layouts.size());

    for(DescriptorSetLayout* layout : _descriptor_set_layouts)
        vk_layouts.push_back(layout->get_handle());

    VkPipelineLayoutCreateInfo create_info =
    {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = static_cast<uint32_t>(vk_layouts.size()),
        .pSetLayouts            = vk_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(_push_constant_ranges.size()),
        .pPushConstantRanges    = _push_constant_ranges.data()
    };

    if(vkCreatePipelineLayout(_context.get_device()->get_handle(), &create_info, nullptr, &_vk_layout) != VK_SUCCESS)
        return false;

    return true;
}

void PipelineLayout::add_push_constant_range(ShaderType type, uint32_t offset, uint32_t size_bytes)
{
    _push_constant_ranges.push_back({ vulkan_helpers::convert_shader_stage(type), offset, size_bytes });
}

void PipelineLayout::add_descriptor_set_layout(DescriptorSetLayout& layout)
{
    _descriptor_set_layouts.push_back(&layout);
}

VkPipelineLayout PipelineLayout::get_handle(void) const
{
    return _vk_layout;
}
