#ifndef REND_FRAME_BUFFER_H
#define REND_FRAME_BUFFER_H

#include "core/rend_constants.h"
#include "core/rend_defs.h"

namespace rend
{

class GPUTexture;
class RenderPass;

class Framebuffer
{
public:
    Framebuffer(void)                          = default;
    ~Framebuffer(void)                         = default;
    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    StatusCode create(const FramebufferInfo& info);
    void       destroy(void);

    FramebufferHandle handle(void) const;
    //RenderPassHandle  render_pass(void) const;
    //Texture2DHandle   depth_buffer(void) const;

    void on_end_render_pass(void);

private:
    FramebufferHandle   _handle{ NULL_HANDLE };
    FramebufferInfo     _framebuffer_info{};
};

}

#endif

