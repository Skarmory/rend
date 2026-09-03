#ifndef REND_CORE_DESCRIPTOR_SET_LAYOUT_H
#define REND_CORE_DESCRIPTOR_SET_LAYOUT_H

#include "rend/rend_api.h"
#include "rend/core/descriptor_update_rate.h"
#include "rend/core/descriptor_set_layout_binding.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_defs.h"
#include "rend/core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

struct DescriptorSetLayoutInfo
{
    DescriptorUpdateRate update_rate;
    std::vector<DescriptorSetLayoutBinding> layout_bindings;
};

class REND_API DescriptorSetLayout : public GPUResource, public RendObject
{
public:
    explicit DescriptorSetLayout(const std::string& name, const DescriptorSetLayoutInfo& info);
    virtual ~DescriptorSetLayout(void) = default;
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    const DescriptorSetLayoutInfo& get_info(void) const;
    DescriptorUpdateRate get_update_rate(void) const;
    const DescriptorSetLayoutBinding* get_layout_binding(uint32_t binding_slot) const;

private:
    const DescriptorSetLayoutInfo _info;
};

}

#endif
