#include "rend/core/descriptor_set.h"

#include "rend/core/descriptor_set_layout.h"

using namespace rend;

DescriptorSet::DescriptorSet(const std::string& name, const DescriptorSetLayout& layout)
    :
        GPUResource(name),
        _layout(layout)
{
}

DescriptorUpdateRate DescriptorSet::get_update_rate(void) const
{
    return _layout.get_update_rate();
}

const std::vector<DescriptorSetBinding>& DescriptorSet::get_bindings(void) const
{
    return _bindings;
}

void DescriptorSet::bind_resource(const DescriptorSetBinding& descriptor)
{
    _bindings.push_back(descriptor);
}
