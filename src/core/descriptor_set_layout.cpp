#include "core/descriptor_set_layout.h"

using namespace rend;

DescriptorSetLayout::DescriptorSetLayout(const std::string& name, const DescriptorSetLayoutInfo& info)
    :
        GPUResource(name),
        _info(info)
{
}

const DescriptorSetLayoutInfo& DescriptorSetLayout::get_info(void) const
{
    return _info;
}

DescriptorFrequency DescriptorSetLayout::get_frequency(void) const
{
    return _info.frequency;
}

const DescriptorSetLayoutBinding* DescriptorSetLayout::get_layout_binding(uint32_t binding_slot) const
{
    for(auto& layout_binding : _info.layout_bindings)
    {
        if(layout_binding.binding == binding_slot)
        {
            return &layout_binding;
        }
    }

    return nullptr;
}
