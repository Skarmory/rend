#ifndef REND_CORE_DESCRIPTOR_SET_LAYOUT_H
#define REND_CORE_DESCRIPTOR_SET_LAYOUT_h

#include "core/descriptor_set_layout_binding.h"
#include "core/rend_defs.h"
#include "core/gpu_resource.h"

#include <string>
#include <vector>

namespace rend
{

struct DescriptorSetLayoutInfo
{
    std::vector<DescriptorSetLayoutBinding> layout_bindings;
};

class DescriptorSetLayout : public GPUResource
{
public:
    explicit DescriptorSetLayout(const std::string& name, const DescriptorSetLayoutInfo& info);
    ~DescriptorSetLayout(void);
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

    DescriptorSetLayoutHandle handle(void) const;

    const DescriptorSetLayoutInfo& get_info(void) const;

private:
    DescriptorSetLayoutHandle _handle{ NULL_HANDLE };
    DescriptorSetLayoutInfo   _info{};
};

}

#endif
