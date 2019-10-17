#include "descriptor_set_layout.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

DescriptorSetLayout::DescriptorSetLayout(DeviceContext& context)
    : _context(context),
      _vk_layout(VK_NULL_HANDLE)
{
}

DescriptorSetLayout::~DescriptorSetLayout(void)
{
    vkDestroyDescriptorSetLayout(_context.get_device()->get_handle(), _vk_layout, nullptr);
}

bool DescriptorSetLayout::create_descriptor_set_layout(void)
{
    if(_vk_layout != VK_NULL_HANDLE)
        return false;

    VkDescriptorSetLayoutCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings = _bindings.data()
    };

    if(vkCreateDescriptorSetLayout(_context.get_device()->get_handle(), &create_info, nullptr, &_vk_layout) != VK_SUCCESS)
        return false;

    return true;
}

void DescriptorSetLayout::add_uniform_buffer_binding(uint32_t slot, ShaderType shader_stage)
{
    _bindings.push_back({
        slot, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, vulkan_helpers::convert_shader_stage(shader_stage), nullptr
    });
}

void DescriptorSetLayout::add_combined_image_sampler_binding(uint32_t slot, ShaderType shader_stage)
{
    _bindings.push_back({
        slot, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, vulkan_helpers::convert_shader_stage(shader_stage), nullptr
    });
}

VkDescriptorSetLayout DescriptorSetLayout::get_handle(void) const
{
    return _vk_layout;
}

const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetLayout::get_layout_bindings(void) const
{
    return _bindings;
}
