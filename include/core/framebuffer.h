#ifndef REND_CORE_FRAME_BUFFER_H
#define REND_CORE_FRAME_BUFFER_H

#include "core/gpu_resource.h"
#include "core/rend_constants.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>
#include <vector>

#include "core/texture_info.h"

namespace rend
{

class RenderPass;

struct FramebufferInfo
{
    uint32_t                 width{ 0 };
    uint32_t                 height{ 0 };
    uint32_t                 depth{ 0 };
    uint32_t                 layers{ 0 };
    bool                     use_size_ratio{ false };
    rend::SizeRatio          size_ratio{ rend::SizeRatio::FULL };
    RenderPass*              render_pass{ nullptr };
    std::vector<std::string> named_render_targets;
    std::vector<TextureInfo> render_targets;
};

class Framebuffer : public GPUResource, public RendObject
{
public:
    Framebuffer(const std::string& name, const FramebufferInfo& info);
    virtual ~Framebuffer(void) = default;
    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    const FramebufferInfo& get_info(void) const;

    const std::vector<TextureInfo>& get_attachment_infos(void) const;
    const std::vector<std::string>& get_attachment_names(void) const;
    //const std::vector<TextureInfo>& get_colour_attachments_info(void) const;
    //const TextureInfo& get_depth_stencil_attachment_info(void) const;

    //const std::vector<std::string>& get_colour_attachment_names(void) const;
    //const std::string& get_depth_stencil_attachment_name(void) const;

private:
    FramebufferInfo _info{};
};

}

#endif

