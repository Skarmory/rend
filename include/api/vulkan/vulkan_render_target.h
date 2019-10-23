#ifdef USE_VULKAN
#ifndef REND_VULKAN_RENDER_TARGET_H
#define REND_VULKAN_RENDER_TARGET_H

#include "vulkan_gpu_texture.h"
#include "rend_defs.h"

namespace rend
{

class DeviceContext;

class VulkanRenderTarget : public VulkanGPUTexture
{
public:
    explicit VulkanRenderTarget(DeviceContext& context);
    ~VulkanRenderTarget(void);

    VulkanRenderTarget(const VulkanRenderTarget&)            = delete;
    VulkanRenderTarget(VulkanRenderTarget&&)                 = delete;
    VulkanRenderTarget& operator=(const VulkanRenderTarget&) = delete;
    VulkanRenderTarget& operator=(VulkanRenderTarget&&)      = delete;

protected:
    StatusCode create_render_target_api(uint32_t width, uint32_t height, TextureFormat format);
};

}

#endif
#endif