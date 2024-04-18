#include "core/descriptor_set_layout.h"

using namespace rend;

DescriptorSetLayout::DescriptorSetLayout(const std::string& name, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle)
{
}
