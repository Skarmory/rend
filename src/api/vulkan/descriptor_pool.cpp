#include "descriptor_pool.h"

#include "device_context.h"
#include "logical_device.h"
#include "descriptor_set_layout.h"
#include "vulkan_gpu_buffer.h"
#include "vulkan_gpu_texture.h"
#include "vulkan_uniform_buffer.h"

using namespace rend;

/* ----- Descriptor Set ----- */

DescriptorSet::DescriptorSet(DeviceContext& context, VkDescriptorSet set)
    : _context(context),
      _vk_set(set)
{
}

VkDescriptorSet DescriptorSet::get_handle(void) const
{
    return _vk_set;
}

void DescriptorSet::describe(uint32_t binding, VulkanGPUTexture* texture)
{
    Binding* _binding = _find_binding(binding);
    if(_binding)
    {
        _binding->type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        _binding->bound_resource = texture;
    }
    else
    {
        _bindings.push_back({
            binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture
        });
    }
}

void DescriptorSet::describe(uint32_t binding, VulkanUniformBuffer* buffer)
{
    Binding* _binding = _find_binding(binding);
    if(_binding)
    {
        _binding->type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _binding->bound_resource = buffer;
    }
    else
    {
        _bindings.push_back({
            binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, buffer
        });
    }
}

void DescriptorSet::update(void)
{
    std::vector<VkWriteDescriptorSet> vk_write_descs;
    std::vector<VkDescriptorImageInfo> vk_image_infos;
    std::vector<VkDescriptorBufferInfo> vk_buffer_infos;

    vk_image_infos.reserve(_bindings.size());
    vk_buffer_infos.reserve(_bindings.size());

    for(Binding& binding : _bindings)
    {
        VkWriteDescriptorSet write_desc = {};
        write_desc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_desc.pNext            = nullptr;
        write_desc.dstSet           = _vk_set;
        write_desc.dstBinding       = binding.slot;
        write_desc.dstArrayElement  = 0;
        write_desc.descriptorCount  = 1;
        write_desc.descriptorType   = binding.type;

        switch(binding.type)
        {
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            {
                VulkanGPUTexture* texture = std::get<VulkanGPUTexture*>(binding.bound_resource);

                uint32_t idx = vk_image_infos.size();

                vk_image_infos.push_back({
                    texture->get_sampler(), texture->get_view(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                });

                write_desc.pImageInfo = &vk_image_infos[idx];

                break;
            }

            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            {
                VulkanGPUBuffer* buffer = std::get<VulkanGPUBuffer*>(binding.bound_resource);

                uint32_t idx = vk_buffer_infos.size();

                vk_buffer_infos.push_back({
                    buffer->get_handle(), 0, buffer->bytes()
                });

                write_desc.pBufferInfo = &vk_buffer_infos[idx];

                break;
            }
        }

        vk_write_descs.push_back(write_desc);
    }

    vkUpdateDescriptorSets(_context.get_device()->get_handle(), vk_write_descs.size(), vk_write_descs.data(), 0, nullptr);
}

DescriptorSet::Binding* DescriptorSet::_find_binding(uint32_t slot)
{
    for(Binding& binding : _bindings)
    {
        if(binding.slot == slot)
        {
            return &binding;
        }
    }

    return nullptr;
}

/* ----- Descriptor Pool ----- */

DescriptorPool::DescriptorPool(DeviceContext& context)
    : _context(context),
      _max_sets(0),
      _vk_pool(VK_NULL_HANDLE)
{
}

DescriptorPool::~DescriptorPool(void)
{
    for(DescriptorSet* dset : _sets)
        delete dset;

    vkDestroyDescriptorPool(_context.get_device()->get_handle(), _vk_pool, nullptr);
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

    if(vkCreateDescriptorPool(_context.get_device()->get_handle(), &create_info, nullptr, &_vk_pool) != VK_SUCCESS)
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
    if((result = vkAllocateDescriptorSets(_context.get_device()->get_handle(), &alloc_info, vk_sets.data())) != VK_SUCCESS)
        return result;

    for(VkDescriptorSet vk_set : vk_sets)
    {
        DescriptorSet* dset = new DescriptorSet(_context, vk_set);
        _sets.push_back(dset);
        out_sets.push_back(dset);
    }

    return result;
}
