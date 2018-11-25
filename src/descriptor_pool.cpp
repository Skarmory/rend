#include "descriptor_pool.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

DescriptorPool::DescriptorPool(LogicalDevice* device, uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& pool_sizes) : _device(device), _max_sets(max_sets), _sets_allocated(0)
{
    VkDescriptorPoolCreateInfo create_info =
    {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .maxSets       = max_sets,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data()
    };

    VULKAN_DEATH_CHECK(vkCreateDescriptorPool(_device->get_handle(), &create_info, nullptr, &_vk_pool), "Failed to create descriptor pool");
}

DescriptorPool::~DescriptorPool(void)
{
    vkDestroyDescriptorPool(_device->get_handle(), _vk_pool, nullptr);
}

VkResult DescriptorPool::allocate(const std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& out_sets)
{
    if(_sets_allocated >= _max_sets)
        return VK_ERROR_OUT_OF_POOL_MEMORY;

    out_sets.resize(layouts.size());

    VkDescriptorSetAllocateInfo alloc_info =
    {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = _vk_pool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts        = layouts.data()
    };

    VkResult result;
    if((result = vkAllocateDescriptorSets(_device->get_handle(), &alloc_info, out_sets.data())) == VK_SUCCESS)
        _sets_allocated++;

    return result;
}
