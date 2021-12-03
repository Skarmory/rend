#ifndef REND_DESCRIPTOR_SET_LAYOUT_H
#define REND_DESCRIPTOR_SET_LAYOUT_h

#include "core/rend_defs.h"

namespace rend
{

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(void) = default;
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    bool create(const DescriptorSetLayoutInfo& info);
    void destroy(void);

    DescriptorSetLayoutHandle handle(void) const;

private:
    DescriptorSetLayoutHandle _handle{ NULL_HANDLE };
};

}

#endif
