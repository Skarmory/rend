#ifndef REND_PIPELINE_LAYOUT_H
#define REND_PIPELINE_LAYOUT_H

#include "core/rend_defs.h"
#include "core/gpu_resource.h"

#include <string>

namespace rend
{

class PipelineLayout : public GPUResource
{
public:
    explicit PipelineLayout(const std::string& name, const PipelineLayoutInfo& info);
    ~PipelineLayout(void);
    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout(PipelineLayout&&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&) = delete;

    PipelineLayoutHandle handle(void) const;

private:
    PipelineLayoutHandle _handle{ NULL_HANDLE };
    PipelineLayoutInfo   _info{};
};

}

#endif
