#ifdef USE_VULKAN
#ifndef REND_VULKAN_DEPTH_BUFFER_H
#define REND_VULKAN_DEPTH_BUFFER_H

#include "vulkan_gpu_texture.h"
#include "rend_defs.h"

namespace rend
{

class DeviceContext;

class VulkanDepthBuffer : public VulkanGPUTexture
{
public:
    explicit VulkanDepthBuffer(DeviceContext& context);
    virtual ~VulkanDepthBuffer(void);

    VulkanDepthBuffer(const VulkanDepthBuffer&)            = delete;
    VulkanDepthBuffer(VulkanDepthBuffer&&)                 = delete;
    VulkanDepthBuffer& operator=(const VulkanDepthBuffer&) = delete;
    VulkanDepthBuffer& operator=(VulkanDepthBuffer&&)      = delete;

protected:
    StatusCode create_depth_buffer_api(uint32_t width, uint32_t height);
};

}

#endif
#endif
