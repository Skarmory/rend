#include "render_target.h"

using namespace rend;

RenderTarget::RenderTarget(DeviceContext& context)
    :
#ifdef USE_VULKAN
        VulkanRenderTarget(context)
#endif
{
}

RenderTarget::~RenderTarget(void)
{
}

bool RenderTarget::create_render_target(uint32_t width, uint32_t height, TextureFormat format)
{
    if(create_render_target_api(width, height, format) != StatusCode::SUCCESS)
    {
        return false;
    }

    return true;
}

void RenderTarget::destroy_render_target(void)
{
    destroy_texture_api();
}
