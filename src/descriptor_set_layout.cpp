#include "descriptor_set_layout.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

DescriptorSetLayout::DescriptorSetLayout(LogicalDevice* device, const std::vector<VkDescriptorSetLayoutBinding>& bindings) : _device(device)
{
    VkDescriptorSetLayoutCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()
    };

    VULKAN_DEATH_CHECK(vkCreateDescriptorSetLayout(_device->get_handle(), &create_info, nullptr, &_vk_layout), "Failed to create descriptor set layout");
}

DescriptorSetLayout::~DescriptorSetLayout(void)
{
    vkDestroyDescriptorSetLayout(_device->get_handle(), _vk_layout, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayout::get_handle(void) const
{
    return _vk_layout;
}
