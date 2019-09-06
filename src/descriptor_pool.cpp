#include "descriptor_pool.h"

#include "logical_device.h"
#include "descriptor_set_layout.h"
#include "utils.h"

#include <algorithm>

using namespace rend;

/* ----- Descriptor Set ----- */

DescriptorSet::DescriptorSet(LogicalDevice* device, VkDescriptorSet set) : _vk_set(set), _device(device)
{
}

VkDescriptorSet DescriptorSet::get_handle(void) const
{
    return _vk_set;
}

void DescriptorSet::describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const std::vector<VkDescriptorImageInfo>& descriptor_infos)
{
    _vk_write_descs.push_back({
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = _vk_set,
        .dstBinding       = binding,
        .dstArrayElement  = array_elem,
        .descriptorCount  = static_cast<uint32_t>(descriptor_infos.size()),
        .descriptorType   = type,
        .pImageInfo       = descriptor_infos.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr
    });
}

void DescriptorSet::describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const std::vector<VkDescriptorBufferInfo>& descriptor_infos)
{
    _vk_write_descs.push_back({
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = _vk_set,
        .dstBinding       = binding,
        .dstArrayElement  = array_elem,
        .descriptorCount  = static_cast<uint32_t>(descriptor_infos.size()),
        .descriptorType   = type,
        .pImageInfo       = nullptr,
        .pBufferInfo      = descriptor_infos.data(),
        .pTexelBufferView = nullptr
    });
}

void DescriptorSet::describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const VkDescriptorBufferInfo* data, uint32_t count)
{
    _vk_write_descs.push_back({
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = _vk_set,
        .dstBinding       = binding,
        .dstArrayElement  = array_elem,
        .descriptorCount  = count,
        .descriptorType   = type,
        .pImageInfo       = nullptr,
        .pBufferInfo      = data,
        .pTexelBufferView = nullptr
    });
}

void DescriptorSet::describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const std::vector<VkBufferView>& descriptor_infos)
{
    _vk_write_descs.push_back({
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = _vk_set,
        .dstBinding       = binding,
        .dstArrayElement  = array_elem,
        .descriptorCount  = static_cast<uint32_t>(descriptor_infos.size()),
        .descriptorType   = type,
        .pImageInfo       = nullptr,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = descriptor_infos.data()
    });
}

void DescriptorSet::update(void)
{
    vkUpdateDescriptorSets(_device->get_handle(), _vk_write_descs.size(), _vk_write_descs.data(), 0, nullptr);

    _vk_write_descs.clear();
}

/* ----- Descriptor Pool ----- */

DescriptorPool::DescriptorPool(LogicalDevice* device, uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& pool_sizes) : _device(device), _max_sets(max_sets)
{
    _sets.reserve(_max_sets);

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
    for(auto dset : _sets)
        delete dset;
    vkDestroyDescriptorPool(_device->get_handle(), _vk_pool, nullptr);
}

VkResult DescriptorPool::allocate(const std::vector<DescriptorSetLayout*>& layouts, std::vector<DescriptorSet*>& out_sets)
{
    if(_sets.size() >= _max_sets)
        return VK_ERROR_OUT_OF_POOL_MEMORY;

    std::vector<VkDescriptorSet> vk_sets;
    vk_sets.resize(layouts.size());
    out_sets.reserve(layouts.size());

    std::vector<VkDescriptorSetLayout> vk_layouts;
    vk_layouts.reserve(layouts.size());

    std::for_each(layouts.begin(), layouts.end(), [&vk_layouts](DescriptorSetLayout* l){ vk_layouts.push_back(l->get_handle()); });

    VkDescriptorSetAllocateInfo alloc_info =
    {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = _vk_pool,
        .descriptorSetCount = static_cast<uint32_t>(vk_layouts.size()),
        .pSetLayouts        = vk_layouts.data()
    };

    VkResult result = VK_SUCCESS;
    if((result = vkAllocateDescriptorSets(_device->get_handle(), &alloc_info, vk_sets.data())) != VK_SUCCESS)
        return result;

    for(auto vk_set : vk_sets)
    {
        DescriptorSet* dset = new DescriptorSet(_device, vk_set);
        _sets.push_back(dset);
        out_sets.push_back(dset);
    }

    return result;
}
