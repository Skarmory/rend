#include "core/gpu_texture.h"

#include "core/device_context.h"

using namespace rend;

GPUTexture::GPUTexture(const TextureInfo& info)
    : _info(info)
{
    auto& ctx = DeviceContext::instance();

    _handle = ctx.create_texture(info.width, info.height, info.depth, info.mips, info.layers, info.format, info.samples, info.usage);
}

GPUTexture::~GPUTexture(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_texture(_handle);
}
