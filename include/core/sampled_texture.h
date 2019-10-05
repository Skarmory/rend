#ifndef REND_SAMPLED_TEXTURE_H
#define REND_SAMPLED_TEXTURE_H

#ifdef USE_VULKAN
#include "vulkan_sampled_texture.h"
#endif

namespace rend
{

class DeviceContext;

#ifdef USE_VULKAN
class SampledTexture : public VulkanSampledTexture
#endif
{
public:
    explicit SampledTexture(DeviceContext& context);
    virtual ~SampledTexture(void);

    SampledTexture(const SampledTexture&)            = delete;
    SampledTexture(SampledTexture&&)                 = delete;
    SampledTexture& operator=(const SampledTexture&) = delete;
    SampledTexture& operator=(SampledTexture&&)      = delete;

    bool create_sampled_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, TextureFormat format);
};

}

#endif
