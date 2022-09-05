#ifndef REND_PIPELINE_H
#define REND_PIPELINE_H

#include "core/rend_defs.h"
#include "core/gpu_resource.h"

#include <string>

namespace rend
{

class Pipeline : public GPUResource
{
public:
    explicit Pipeline(const std::string& name, const PipelineInfo& info);
    ~Pipeline(void);
    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;

    PipelineHandle handle(void) const;

private:
    PipelineHandle _handle{ NULL_HANDLE };
    PipelineInfo _info{};
};

}

#endif
