#ifndef REND_SAMPLED_TEXTURE_H
#define REND_SAMPLED_TEXTURE_H

#include "gpu_texture_base.h"

namespace rend
{

class SampledTexture : public GPUTextureBase
{
public:
    SampledTexture(void) = default;
    ~SampledTexture(void) = default;

    SampledTexture(const SampledTexture&)            = delete;
    SampledTexture(SampledTexture&&)                 = delete;
    SampledTexture& operator=(const SampledTexture&) = delete;
    SampledTexture& operator=(SampledTexture&&)      = delete;

    bool create_sampled_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, Format format);
};

}

#endif
