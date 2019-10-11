#ifndef REND_RENDER_TARGET_H
#define REND_RENDER_TARGET_H

#ifdef USE_VULKAN
#include "vulkan_render_target.h"
#endif

namespace rend
{

class DeviceContext;

#ifdef USE_VULKAN
class RenderTarget : public VulkanRenderTarget
#endif
{
public:
    explicit RenderTarget(DeviceContext& context);
    ~RenderTarget(void);

    RenderTarget(const RenderTarget&)            = delete;
    RenderTarget(RenderTarget&&)                 = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;
    RenderTarget& operator=(RenderTarget&&)      = delete;

    bool create_render_target(uint32_t width, uint32_t height, TextureFormat format);
    void destroy_render_target(void);
};

}

#endif
