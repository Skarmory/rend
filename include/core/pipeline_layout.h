#ifndef REND_CORE_PIPELINE_LAYOUT_H
#define REND_CORE_PIPELINE_LAYOUT_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>

namespace rend
{

class DescriptorSetLayout;
struct PushConstantRange;

struct PipelineLayoutInfo
{
    std::vector<DescriptorSetLayout*> descriptor_set_layouts;
    std::vector<PushConstantRange> push_constant_ranges;
};

class PipelineLayout : public GPUResource, public RendObject
{
public:
    PipelineLayout(const std::string& name);
    virtual ~PipelineLayout(void) = default;
};

}

#endif
