#ifndef REND_DESCRIPTOR_SET_H
#define REND_DESCRIPTOR_SET_H

#include "core/rend_defs.h"

namespace rend
{

class DescriptorSet
{
public:
    explicit DescriptorSet(const DescriptorSetInfo& info);
    ~DescriptorSet(void);
    DescriptorSet(const DescriptorSet&)            = delete;
    DescriptorSet(DescriptorSet&&)                 = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&)      = delete;

    void add_binding(const rend::DescriptorSetBinding& binding) const;

    DescriptorSetHandle handle(void) const;

private:
    DescriptorSetHandle  _handle{ NULL_HANDLE };
};

}

#endif
