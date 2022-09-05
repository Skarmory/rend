#ifndef REND_FRAME_BUFFER_H
#define REND_FRAME_BUFFER_H

#include "core/gpu_resource.h"
#include "core/rend_constants.h"
#include "core/rend_defs.h"

#include <string>
#include <vector>

namespace rend
{

class GPUTexture;
class RenderPass;

struct FramebufferInfo
{
    uint32_t                   width{ 0 };
    uint32_t                   height{ 0 };
    uint32_t                   depth{ 0 };
    RenderPassHandle           render_pass{ NULL_HANDLE };
    TextureHandle              depth_target{ NULL_HANDLE };
    std::vector<TextureHandle> render_targets;
};

class Framebuffer : GPUResource
{
public:
    explicit Framebuffer(const std::string& name, const FramebufferInfo& info, const FramebufferInfo& api_info);
    ~Framebuffer(void);
    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    FramebufferHandle handle(void) const;
    const std::vector<TextureHandle>& get_colour_attachments(void) const;
    TextureHandle get_depth_stencil_attachment(void) const;

private:
    FramebufferHandle _handle{ NULL_HANDLE };
    FramebufferInfo   _info{};
};

}

#endif

