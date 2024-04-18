#ifndef REND_CORE_DESCRIPTOR_SET_BINDING_H
#define REND_CORE_DESCRIPTOR_SET_BINDING_H

#include "core/rend_defs.h"

namespace rend
{

struct DescriptorSetBinding
{
    uint32_t       slot;
    DescriptorType type;
    void*          resource;
};

}

#endif
