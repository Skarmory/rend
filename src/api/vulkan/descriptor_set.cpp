#include "descriptor_set.h"

#include "device_context.h"
#include "logical_device.h"
#include "uniform_buffer.h"
#include "vulkan_gpu_buffer.h"
#include "vulkan_gpu_texture.h"
#include "vulkan_device_context.h"

using namespace rend;

DescriptorSet::DescriptorSet(VkDescriptorSet set)
    :
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

void DescriptorSet::describe(uint32_t binding, UniformBuffer* buffer)
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
                UniformBuffer* buffer = std::get<UniformBuffer*>(binding.bound_resource);

                uint32_t idx = vk_buffer_infos.size();

                auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

                vk_buffer_infos.push_back({
                    ctx.get_buffer(buffer->get_handle()), 0, buffer->bytes()
                });

                write_desc.pBufferInfo = &vk_buffer_infos[idx];

                break;
            }
        }

        vk_write_descs.push_back(write_desc);
    }

    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->update_descriptor_sets(vk_write_descs);
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
