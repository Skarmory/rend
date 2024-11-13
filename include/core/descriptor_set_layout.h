#ifndef REND_CORE_DESCRIPTOR_SET_LAYOUT_H
#define REND_CORE_DESCRIPTOR_SET_LAYOUT_H

#include "core/descriptor_frequency.h"
#include "core/descriptor_set_layout_binding.h"
#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

struct DescriptorSetLayoutInfo
{
    DescriptorFrequency frequency;
    std::vector<DescriptorSetLayoutBinding> layout_bindings;
};

class DescriptorSetLayout : public GPUResource, public RendObject
{
public:
    explicit DescriptorSetLayout(const std::string& name, const DescriptorSetLayoutInfo& info);
    virtual ~DescriptorSetLayout(void) = default;
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    const DescriptorSetLayoutInfo& get_info(void) const;
    DescriptorFrequency get_frequency(void) const;
    const DescriptorSetLayoutBinding* get_layout_binding(uint32_t binding_slot) const;

private:
    const DescriptorSetLayoutInfo _info;
};

}

#endif
