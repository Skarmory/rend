#include "core/framebuffer.h"

#include "core/device_context.h"
#include "core/gpu_texture.h"
#include "core/render_pass.h"
#include "core/rend_service.h"

#include <cassert>
#include <functional>

using namespace rend;

Framebuffer::Framebuffer(const std::string& name, const FramebufferInfo& info, const FramebufferInfo& api_info)
    : GPUResource(name),
      _info(info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_framebuffer(api_info);
}

Framebuffer::~Framebuffer(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_framebuffer(_handle);
}

FramebufferHandle Framebuffer::handle(void) const
{
    return _handle;
}

const std::vector<TextureHandle>& Framebuffer::get_colour_attachments(void) const
{
    return _info.render_targets;
}

TextureHandle Framebuffer::get_depth_stencil_attachment(void) const
{
    return _info.depth_target;
}
