#include "descriptor_pool.h"

#include "descriptor_set.h"
#include "descriptor_set_layout.h"
#include "device_context.h"
#include "logical_device.h"

using namespace rend;

DescriptorPool::DescriptorPool(DeviceContext& context)
    : _context(context),
      _max_sets(0),
      _sampler_count(0),
      _combined_image_sampler_count(0),
      _sampled_image_count(0),
      _storage_image_count(0),
      _uniform_texel_buffer_count(0),
      _storage_texel_buffer_count(0),
      _uniform_buffer_count(0),
      _storage_buffer_count(0),
      _dynamic_uniform_buffer_count(0),
      _dynamic_storage_buffer_count(0),
      _input_attachment_count(0),
      _vk_pool(VK_NULL_HANDLE)
{
}

DescriptorPool::~DescriptorPool(void)
{
    for(DescriptorSet* dset : _sets)
        delete dset;

    vkDestroyDescriptorPool(_context.get_device()->get_handle(), _vk_pool, nullptr);
}

bool DescriptorPool::create_descriptor_pool(uint32_t max_sets)
{
    if(_vk_pool != VK_NULL_HANDLE)
        return false;

    std::vector<VkDescriptorPoolSize> pool_sizes;

    if(_sampler_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLER, _sampler_count });
    if(_combined_image_sampler_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _combined_image_sampler_count });
    if(_sampled_image_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, _sampled_image_count });
    if(_storage_image_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, _storage_image_count });
    if(_uniform_texel_buffer_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, _uniform_texel_buffer_count});
    if(_storage_texel_buffer_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, _storage_texel_buffer_count});
    if(_uniform_buffer_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _uniform_buffer_count});
    if(_storage_buffer_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, _storage_buffer_count});
    if(_dynamic_uniform_buffer_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, _dynamic_uniform_buffer_count});
    if(_dynamic_storage_buffer_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, _dynamic_storage_buffer_count});
    if(_input_attachment_count > 0)
        pool_sizes.push_back({ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _input_attachment_count});

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

    if(vkCreateDescriptorPool(_context.get_device()->get_handle(), &create_info, nullptr, &_vk_pool) != VK_SUCCESS)
        return false;

    _max_sets = max_sets;

    return true;
}

void DescriptorPool::set_sampler_count(uint32_t count)
{
    _sampler_count = count;
}

void DescriptorPool::set_combined_image_sampler_count(uint32_t count)
{
    _combined_image_sampler_count = count;
}

void DescriptorPool::set_sampled_image_count(uint32_t count)
{
    _sampled_image_count = count;
}

void DescriptorPool::set_storage_image_count(uint32_t count)
{
    _storage_image_count = count;
}

void DescriptorPool::set_uniform_texel_buffer_count(uint32_t count)
{
    _uniform_texel_buffer_count = count;
}

void DescriptorPool::set_storage_texel_buffer_count(uint32_t count)
{
    _storage_texel_buffer_count = count;
}

void DescriptorPool::set_uniform_buffer_count(uint32_t count)
{
    _uniform_buffer_count = count;
}

void DescriptorPool::set_storage_buffer_count(uint32_t count)
{
    _storage_buffer_count = count;
}

void DescriptorPool::set_dynamic_uniform_buffer_count(uint32_t count)
{
    _dynamic_uniform_buffer_count = count;
}

void DescriptorPool::set_dynamic_storage_buffer_count(uint32_t count)
{
    _dynamic_storage_buffer_count = count;
}

void DescriptorPool::set_input_attachment_count(uint32_t count)
{
    _input_attachment_count = count;
}

std::vector<DescriptorSet*> DescriptorPool::allocate(const std::vector<DescriptorSetLayout*>& layouts)
{
    if(_sets.size() >= _max_sets)
        return {};

    std::vector<DescriptorSet*> out_sets;
    out_sets.reserve(layouts.size());

    std::vector<VkDescriptorSet> vk_sets;
    vk_sets.resize(layouts.size());

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

    if(vkAllocateDescriptorSets(_context.get_device()->get_handle(), &alloc_info, vk_sets.data()) != VK_SUCCESS)
        return {};

    for(VkDescriptorSet vk_set : vk_sets)
    {
        DescriptorSet* dset = new DescriptorSet(_context, vk_set);
        _sets.push_back(dset);
        out_sets.push_back(dset);
    }

    return out_sets;
}
