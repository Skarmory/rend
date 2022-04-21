#ifndef REND_DESCRIPTOR_POOL_H
#define REND_DESCRIPTOR_POOL_H

#include "core/rend_defs.h"

namespace rend
{

class DescriptorPool
{
public:
    explicit DescriptorPool(const DescriptorPoolInfo& info);
    ~DescriptorPool(void);
    DescriptorPool(const DescriptorPool&)            = delete;
    DescriptorPool(DescriptorPool&&)                 = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&)      = delete;

    DescriptorPoolHandle handle(void) const;

private:
    DescriptorPoolHandle _handle{ NULL_HANDLE };
};

}

#endif
