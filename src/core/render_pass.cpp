#include "core/render_pass.h"

#include "core/device_context.h"

#include <cassert>

using namespace rend;

RenderPass::RenderPass(const RenderPassInfo& info)
    : _info(info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_render_pass(info);
}

RenderPass::~RenderPass(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_render_pass(_handle);
}

RenderPassHandle RenderPass::handle(void) const
{
    return _handle;
}
