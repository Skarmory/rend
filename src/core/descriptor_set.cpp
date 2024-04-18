#include "core/descriptor_set.h"

#include "core/device_context.h"

using namespace rend;

DescriptorSet::DescriptorSet(const std::string& name, const DescriptorSetInfo& info, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _set(info.set),
        _layout(info.layout)
{
}

void DescriptorSet::add_uniform_buffer_binding(uint32_t slot, GPUBuffer* buffer)
{
    _bindings.emplace_back(
        slot, DescriptorType::UNIFORM_BUFFER, buffer
    );
}

void DescriptorSet::add_texture_binding(uint32_t slot, GPUTexture* texture)
{
    _bindings.emplace_back(
        slot, DescriptorType::COMBINED_IMAGE_SAMPLER, texture
    );
}

uint32_t DescriptorSet::set(void) const
{
    return _set;
}

const std::vector<DescriptorSetBinding>& DescriptorSet::bindings(void) const
{
    return _bindings;
}
