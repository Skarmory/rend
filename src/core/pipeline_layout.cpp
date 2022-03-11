#include "core/pipeline_layout.h"

#include "core/device_context.h"

using namespace rend;

bool PipelineLayout::create(const PipelineLayoutInfo& info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_pipeline_layout(info);
    
    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    return true;
}

void PipelineLayout::destroy(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_pipeline_layout(_handle);
    _handle = NULL_HANDLE;
}

PipelineLayoutHandle PipelineLayout::handle(void) const
{
    return _handle;
}
