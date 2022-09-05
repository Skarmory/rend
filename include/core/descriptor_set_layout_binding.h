#ifndef REND_CORE_DESCRIPTOR_SET_LAYOUT_BINDING_H
#define REND_CORE_DESCRIPTOR_SET_LAYOUT_BINDING_H

#include "core/rend_defs.h"

namespace rend
{

struct DescriptorSetLayoutBinding
{
    uint32_t       binding{ 0 };
    DescriptorType descriptor_type;
    uint32_t       descriptor_count{ 0 };
    ShaderStages   shader_stages{ 0 };
};

}

#endif
