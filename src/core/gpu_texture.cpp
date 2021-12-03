#include "core/gpu_texture.h"

#include "core/device_context.h"

using namespace rend;

bool GPUTexture::create(const TextureInfo& info)
{
    auto& ctx = DeviceContext::instance();

    _handle = ctx.create_texture(info.width, info.height, info.depth, info.mips, info.layers, info.format, info.samples, info.usage);

    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    _width   = info.width;
    _height  = info.height;
    _depth   = info.depth;
    _mips    = info.mips;
    _layers  = info.layers;
    _format  = info.format;
    _layout  = info.layout;
    _samples = info.samples;
    _usage   = info.usage;

    return true;
}

void GPUTexture::destroy(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_texture(_handle);

    _handle = NULL_HANDLE;
    _width = 0;
    _height = 0;
    _depth = 0;
    _mips = 0;
    _layers = 0;
    _format = Format::R8G8B8A8;
    _layout = ImageLayout::UNDEFINED;
    _samples = MSAASamples::MSAA_1X;
    _usage = ImageUsage::NONE;
    _bytes = 0;
}
