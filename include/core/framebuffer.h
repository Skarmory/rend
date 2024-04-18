#ifndef REND_CORE_FRAME_BUFFER_H
#define REND_CORE_FRAME_BUFFER_H

#include "core/gpu_resource.h"
#include "core/rend_constants.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

class GPUTexture;
class RenderPass;

struct FramebufferInfo
{
    uint32_t                 width{ 0 };
    uint32_t                 height{ 0 };
    uint32_t                 depth{ 0 };
    RenderPass*              render_pass{ nullptr };
    GPUTexture*              depth_target{ nullptr };
    std::vector<GPUTexture*> render_targets;
};

class Framebuffer : public GPUResource, public RendObject
{
public:
    explicit Framebuffer(const std::string& name, const FramebufferInfo& info, RendHandle rend_handle);
    virtual ~Framebuffer(void) = default;
    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    const std::vector<GPUTexture*>& get_colour_attachments(void) const;
    GPUTexture* get_depth_stencil_attachment(void) const;

private:
    FramebufferInfo _info{};
};

}

#endif

