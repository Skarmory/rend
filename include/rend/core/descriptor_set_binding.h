#ifndef REND_CORE_DESCRIPTOR_SET_BINDING_H
#define REND_CORE_DESCRIPTOR_SET_BINDING_H

#include "rend/rend_api.h"
#include "rend/core/rend_defs.h"

namespace rend
{

class GPUResource;

struct DescriptorSetBinding
{
    uint32_t       slot { 0 };
    DescriptorType type;
    GPUResource*   resource{ nullptr };
};

}

#endif
