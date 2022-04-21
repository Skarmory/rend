#include "core/framebuffer.h"

#include "core/device_context.h"
#include "core/gpu_texture.h"
#include "core/render_pass.h"

#include <cassert>

using namespace rend;

Framebuffer::Framebuffer(const FramebufferInfo& info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_framebuffer(info);
    _framebuffer_info = info;
}

Framebuffer::~Framebuffer(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_framebuffer(_handle);
}

FramebufferHandle Framebuffer::handle(void) const
{
    return _handle;
}
