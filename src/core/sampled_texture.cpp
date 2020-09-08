#include "sampled_texture.h"
#include "rend_defs.h"
#include "device_context.h"

using namespace rend;

bool SampledTexture::create_sampled_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mip_levels, Format format)
{
    auto& ctx = DeviceContext::instance();

    // TODO: Replace with generic create texture function
    _handle = ctx.create_texture_2d(width, height, mip_levels, 1, format, ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST);

    if (_handle != NULL_HANDLE)
    {
        _width  = width;
        _height = height;
        _depth  = depth;
        _layers = 1;
        _format = format;
        _layout = ImageLayout::UNDEFINED;
        _samples = MSAASamples::MSAA_1X;

        return true;
    }

    return false;
}
