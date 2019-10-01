#include "descriptor_pool.h"

#include "device_context.h"
#include "logical_device.h"
#include "descriptor_set_layout.h"

using namespace rend;

/* ----- Descriptor Set ----- */

DescriptorSet::DescriptorSet(DeviceContext* context, VkDescriptorSet set)
    : _context(context),
      _vk_set(set)
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
    vkUpdateDescriptorSets(_context->get_device()->get_handle(), _vk_write_descs.size(), _vk_write_descs.data(), 0, nullptr);

    _vk_write_descs.clear();
}

/* ----- Descriptor Pool ----- */

DescriptorPool::DescriptorPool(DeviceContext* context)
    : _context(context),
      _max_sets(0),
      _vk_pool(VK_NULL_HANDLE)
{
}

DescriptorPool::~DescriptorPool(void)
{
    for(DescriptorSet* dset : _sets)
        delete dset;

    vkDestroyDescriptorPool(_context->get_device()->get_handle(), _vk_pool, nullptr);
}

bool DescriptorPool::create_descriptor_pool(uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& pool_sizes)
{
    if(_vk_pool != VK_NULL_HANDLE)
        return false;

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

    if(vkCreateDescriptorPool(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_pool) != VK_SUCCESS)
        return false;

    _max_sets = max_sets;

    return true;
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

    for(DescriptorSetLayout* layout : layouts)
        vk_layouts.push_back(layout->get_handle());

    VkDescriptorSetAllocateInfo alloc_info =
    {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = _vk_pool,
        .descriptorSetCount = static_cast<uint32_t>(vk_layouts.size()),
        .pSetLayouts        = vk_layouts.data()
    };

    VkResult result = VK_SUCCESS;
    if((result = vkAllocateDescriptorSets(_context->get_device()->get_handle(), &alloc_info, vk_sets.data())) != VK_SUCCESS)
        return result;

    for(VkDescriptorSet vk_set : vk_sets)
    {
        DescriptorSet* dset = new DescriptorSet(_context, vk_set);
        _sets.push_back(dset);
        out_sets.push_back(dset);
    }

    return result;
}
