#include "pipeline_layout.h"

#include "device_context.h"
#include "logical_device.h"
#include "descriptor_set_layout.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

#include <cassert>

using namespace rend;

StatusCode PipelineLayout::create(void)
{
    assert(_vk_layout != VK_NULL_HANDLE && "Attempt to create a PipelineLayout that has already been created.");

    std::vector<VkDescriptorSetLayout> vk_layouts;
    vk_layouts.reserve(_descriptor_set_layouts.size());

    for(DescriptorSetLayout* layout : _descriptor_set_layouts)
    {
        vk_layouts.push_back(layout->get_handle());
    }

    VkPipelineLayoutCreateInfo create_info = vulkan_helpers::gen_pipeline_layout_create_info();
    create_info.setLayoutCount = static_cast<uint32_t>(vk_layouts.size());
    create_info.pSetLayouts = vk_layouts.data();
    create_info.pushConstantRangeCount = static_cast<uint32_t>(_push_constant_ranges.size());
    create_info.pPushConstantRanges = _push_constant_ranges.data();

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_layout = ctx.get_device()->create_pipeline_layout(create_info);

    if(_vk_layout == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

void PipelineLayout::destroy(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_pipeline_layout(_vk_layout);
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
