#ifndef REND_RENDER_TARGET_H
#define REND_RENDER_TARGET_H

#include "gpu_texture_base.h"

namespace rend
{

// TODO: Does this really need to be its own class? Probably not.. Get rid of it once
// initial refactoring is complete

class RenderTarget : public GPUTextureBase
{
public:
    RenderTarget(void) = default;
    ~RenderTarget(void) = default;

    RenderTarget(const RenderTarget&)            = delete;
    RenderTarget(RenderTarget&&)                 = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;
    RenderTarget& operator=(RenderTarget&&)      = delete;

    bool create_render_target(uint32_t width, uint32_t height, Format format, MSAASamples samples);
};

}

#endif
