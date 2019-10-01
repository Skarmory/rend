#include "descriptor_set_layout.h"

#include "device_context.h"
#include "logical_device.h"
#include "utils.h"

using namespace rend;

DescriptorSetLayout::DescriptorSetLayout(DeviceContext* context)
    : _context(context),
      _vk_layout(VK_NULL_HANDLE)
{
}

DescriptorSetLayout::~DescriptorSetLayout(void)
{
    vkDestroyDescriptorSetLayout(_context->get_device()->get_handle(), _vk_layout, nullptr);
}

bool DescriptorSetLayout::create_descriptor_set_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    if(_vk_layout != VK_NULL_HANDLE)
        return false;

    VkDescriptorSetLayoutCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()
    };

    if(vkCreateDescriptorSetLayout(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_layout) != VK_SUCCESS)
        return false;

    _bindings = bindings;

    return true;
}

VkDescriptorSetLayout DescriptorSetLayout::get_handle(void) const
{
    return _vk_layout;
}

const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetLayout::get_layout_bindings(void) const
{
    return _bindings;
}
