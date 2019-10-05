#ifndef REND_TEXTURE_H
#define REND_TEXTURE_H

#include "vulkan_gpu_texture.h"
#include "rend_defs.h"

namespace rend
{

class VulkanSampledTexture : public VulkanGPUTexture
{
public:
    explicit VulkanSampledTexture(DeviceContext& context);
    virtual ~VulkanSampledTexture(void);

    VulkanSampledTexture(const VulkanSampledTexture&)            = delete;
    VulkanSampledTexture(VulkanSampledTexture&&)                 = delete;
    VulkanSampledTexture& operator=(const VulkanSampledTexture&) = delete;
    VulkanSampledTexture& operator=(VulkanSampledTexture&&)      = delete;

protected:
    StatusCode create_sampled_texture_api(uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, TextureFormat format);
};

}

#endif
