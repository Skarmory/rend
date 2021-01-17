#include "descriptor_pool.h"

#include "descriptor_set.h"
#include "descriptor_set_layout.h"
#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

#include <cassert>

using namespace rend;

namespace
{
    const size_t c_descriptor_types = 11;
}

StatusCode DescriptorPool::create(uint32_t max_sets)
{
    if(_vk_pool != VK_NULL_HANDLE)
    {
        return StatusCode::ALREADY_CREATED;
    }

    std::vector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.reserve(c_descriptor_types);

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

    VkDescriptorPoolCreateInfo create_info = vulkan_helpers::gen_descriptor_pool_create_info();
    create_info.maxSets       = max_sets;
    create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    create_info.pPoolSizes    = pool_sizes.data();

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_pool = ctx.get_device()->create_descriptor_pool(create_info);
    if(_vk_pool == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    _max_sets = max_sets;
    _sets.reserve(_max_sets);

    return StatusCode::SUCCESS;
}

void DescriptorPool::destroy(void)
{
    for(DescriptorSet* dset : _sets)
    {
        delete dset;
    }

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_descriptor_pool(_vk_pool);
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
    assert(_sets.size() < _max_sets && "Attempt to allocate a DescriptorSet from a fully allocated DescriptorPool");
    assert((_sets.size() + layouts.size()) <= _max_sets && "Attempt to allocate more DescriptorSets than a DescriptorPool has free slots");

    std::vector<DescriptorSet*> out_sets;
    out_sets.reserve(layouts.size());

    std::vector<VkDescriptorSet> vk_sets;
    vk_sets.resize(layouts.size());

    std::vector<VkDescriptorSetLayout> vk_layouts;
    vk_layouts.reserve(layouts.size());

    for(DescriptorSetLayout* layout : layouts)
    {
        vk_layouts.push_back(layout->get_handle());
    }

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    vk_sets = ctx.get_device()->allocate_descriptor_sets(vk_layouts, _vk_pool);

    for(VkDescriptorSet vk_set : vk_sets)
    {
        DescriptorSet* dset = new DescriptorSet(vk_set);
        _sets.push_back(dset);
        out_sets.push_back(dset);
    }

    return out_sets;
}
