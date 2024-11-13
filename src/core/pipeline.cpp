#include "core/pipeline.h"

using namespace rend;

Pipeline::Pipeline(const std::string& name, const PipelineInfo& info)
    :
        GPUResource(name),
        _info(info)
{
}

const ShaderSet& Pipeline::get_shader_set(void) const
{
    return *_info.shader_set;
}

const PipelineInfo& Pipeline::pipeline_info(void) const
{
    return _info;
}
