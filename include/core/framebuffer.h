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
    explicit Framebuffer(const FramebufferInfo& info);
    ~Framebuffer(void);
    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    FramebufferHandle handle(void) const;

private:
    FramebufferHandle   _handle{ NULL_HANDLE };
    FramebufferInfo     _framebuffer_info{};
};

}

#endif

