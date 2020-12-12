#include "gpu_texture.h"

#include "device_context.h"

using namespace rend;

GPUTexture::~GPUTexture(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_texture(_handle);
}

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
