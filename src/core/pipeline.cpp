#include "core/pipeline.h"

#include "core/device_context.h"

using namespace rend;

bool Pipeline::create(const PipelineInfo& info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_pipeline(info);

    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    return true;
}

void Pipeline::destroy(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_pipeline(_handle);
    _handle = NULL_HANDLE;
}

PipelineHandle Pipeline::handle(void) const
{
    return _handle;
}
