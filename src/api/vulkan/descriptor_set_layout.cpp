#include "descriptor_set_layout.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

#include <cassert>

using namespace rend;

StatusCode DescriptorSetLayout::create(void)
{
    assert(_vk_layout == VK_NULL_HANDLE && "Attempt to create a DescriptorSetLayout that has already been created.");

    VkDescriptorSetLayoutCreateInfo create_info = vulkan_helpers::gen_descriptor_set_layout_create_info();
    create_info.bindingCount = static_cast<uint32_t>(_bindings.size());
    create_info.pBindings    = _bindings.data();

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_layout = ctx.get_device()->create_descriptor_set_layout(create_info);

    if(_vk_layout == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

void DescriptorSetLayout::destroy(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_descriptor_set_layout(_vk_layout);
    _vk_layout = VK_NULL_HANDLE;
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
