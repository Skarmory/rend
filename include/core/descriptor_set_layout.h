#ifndef REND_DESCRIPTOR_SET_LAYOUT_H
#define REND_DESCRIPTOR_SET_LAYOUT_h

#include "core/rend_defs.h"

namespace rend
{

class DescriptorSetLayout
{
public:
    explicit DescriptorSetLayout(const DescriptorSetLayoutInfo& info);
    ~DescriptorSetLayout(void);
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    DescriptorSetLayoutHandle handle(void) const;

private:
    DescriptorSetLayoutHandle _handle{ NULL_HANDLE };
};

}

#endif
