#include "render_target.h"

#include "device_context.h"
#include "vulkan_device_context.h"

using namespace rend;

bool RenderTarget::create_render_target(uint32_t width, uint32_t height, Format format, MSAASamples samples)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    _handle = ctx.create_texture_2d(width, height, 1, 1, format, ImageUsage::COLOUR_ATTACHMENT);

    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    _width = width;
    _height = height;
    _format = format;
    _samples = samples;

    return true;
}
