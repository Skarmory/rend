#include "core/pipeline.h"

#include "device_context.h"

using namespace rend;

void Pipeline::create(const PipelineInfo& info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_pipeline(info);
}

void Pipeline::destroy(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_pipeline(_handle);
    _handle = NULL_HANDLE;
}
