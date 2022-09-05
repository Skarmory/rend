#include "core/pipeline.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

Pipeline::Pipeline(const std::string& name, const PipelineInfo& info)
    :
        GPUResource(name),
        _info(info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_pipeline(info);
}

Pipeline::~Pipeline(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_pipeline(_handle);
}

PipelineHandle Pipeline::handle(void) const
{
    return _handle;
}
