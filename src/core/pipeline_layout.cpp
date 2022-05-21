#include "core/pipeline_layout.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

PipelineLayout::PipelineLayout(const PipelineLayoutInfo& info)
    : _info(info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_pipeline_layout(info);
}

PipelineLayout::~PipelineLayout(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_pipeline_layout(_handle);
}

PipelineLayoutHandle PipelineLayout::handle(void) const
{
    return _handle;
}
