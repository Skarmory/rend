#include "core/framebuffer.h"

#include "core/gpu_texture.h"
#include "core/renderer.h"
#include "core/logging/log_defs.h"
#include "core/logging/log_helper_funcs.h"
#include "core/logging/log_manager.h"

using namespace rend;

Framebuffer::Framebuffer(const std::string& name, const FramebufferInfo& info)
    :
        GPUResource(name),
        _info(info)
{
#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "FRAMEBUFFER | Create framebuffer (" + name + ") with params: " + core::logging::to_string(_info));
#endif
}

const FramebufferInfo& Framebuffer::get_info(void) const
{
    return _info;
}

const std::vector<TextureInfo>& Framebuffer::get_attachment_infos(void) const
{
    return _info.render_targets;
}

const std::vector<std::string>& Framebuffer::get_attachment_names(void) const
{
    return _info.named_render_targets;
}

//const std::vector<TextureInfo>& Framebuffer::get_colour_attachments_info(void) const
//{
//    return _info.render_targets;
//}

//const TextureInfo& Framebuffer::get_depth_stencil_attachment_info(void) const
//{
//    return _info.depth_target;
//}

//const std::vector<std::string>& Framebuffer::get_colour_attachment_names(void) const
//{
//    return _info.named_render_targets;
//}

//const std::string& Framebuffer::get_depth_stencil_attachment_name(void) const
//{
//    return _info.named_depth_target;
//}
