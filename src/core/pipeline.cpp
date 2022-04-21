#include "core/pipeline.h"

#include "core/device_context.h"

using namespace rend;

Pipeline::Pipeline(const PipelineInfo& info)
    : _info(info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_pipeline(info);
}

Pipeline::~Pipeline(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_pipeline(_handle);
}

PipelineHandle Pipeline::handle(void) const
{
    return _handle;
}
