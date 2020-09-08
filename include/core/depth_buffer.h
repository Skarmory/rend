#ifndef REND_DEPTH_BUFFER_H
#define REND_DEPTH_BUFFER_H

#include "gpu_texture_base.h"

namespace rend
{

// TODO: Does this really need to be its own class? Probably not.. Get rid of it once
// initial refactoring is complete
class DepthBuffer : public GPUTextureBase
{
public:
    DepthBuffer(void) = default;
    ~DepthBuffer(void) = default;

    DepthBuffer(const DepthBuffer&)            = delete;
    DepthBuffer(DepthBuffer&&)                 = delete;
    DepthBuffer& operator=(const DepthBuffer&) = delete;
    DepthBuffer& operator=(DepthBuffer&&)      = delete;

    bool create_depth_buffer(uint32_t width, uint32_t height);
};

}

#endif
