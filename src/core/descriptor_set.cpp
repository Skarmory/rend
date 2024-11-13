#include "core/descriptor_set.h"

#include "core/descriptor_set_layout.h"

using namespace rend;

DescriptorSet::DescriptorSet(const std::string& name, const DescriptorSetLayout& layout)
    :
        GPUResource(name),
        _layout(layout)
{
}

DescriptorFrequency DescriptorSet::get_index(void) const
{
    return _layout.get_frequency();
}

const std::vector<DescriptorSetBinding>& DescriptorSet::get_bindings(void) const
{
    return _bindings;
}

void DescriptorSet::bind_resource(const DescriptorSetBinding& descriptor)
{
    _bindings.push_back(descriptor);
}
