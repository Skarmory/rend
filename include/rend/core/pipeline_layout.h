#ifndef REND_CORE_PIPELINE_LAYOUT_H
#define REND_CORE_PIPELINE_LAYOUT_H

#include "rend/rend_api.h"
#include "rend/core/descriptor_update_rate.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_defs.h"
#include "rend/core/rend_object.h"

#include <array>
#include <string>

// TODO: Fold this entirely into shader set?

namespace rend
{

class DescriptorSetLayout;
struct PushConstantRange;

struct PipelineLayoutInfo
{
    std::array<DescriptorSetLayout*, (int)DescriptorUpdateRate::Count> descriptor_set_layouts{ nullptr, nullptr, nullptr, nullptr, nullptr };
    std::vector<PushConstantRange> push_constant_ranges;
};

class REND_API PipelineLayout : public GPUResource, public RendObject
{
public:
    PipelineLayout(const std::string& name);
    virtual ~PipelineLayout(void) = default;
};

}

#endif
