#include "core/pipeline.h"

using namespace rend;

Pipeline::Pipeline(const std::string& name, const PipelineInfo& info, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _info(info)
{
}

const PipelineInfo& Pipeline::pipeline_info(void) const
{
    return _info;
}
