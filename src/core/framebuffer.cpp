#include "core/framebuffer.h"

#include "core/gpu_texture.h"

using namespace rend;

Framebuffer::Framebuffer(const std::string& name, const FramebufferInfo& info, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _info(info)
{
}

const std::vector<GPUTexture*>& Framebuffer::get_colour_attachments(void) const
{
    return _info.render_targets;
}

GPUTexture* Framebuffer::get_depth_stencil_attachment(void) const
{
    return _info.depth_target;
}
