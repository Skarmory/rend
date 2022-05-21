#include "core/render_pass.h"

#include "core/device_context.h"
#include "core/rend_service.h"

#include <cassert>

using namespace rend;

RenderPass::RenderPass(const RenderPassInfo& info)
    : _info(info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_render_pass(info);
}

RenderPass::~RenderPass(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_render_pass(_handle);
}

RenderPassHandle RenderPass::handle(void) const
{
    return _handle;
}
