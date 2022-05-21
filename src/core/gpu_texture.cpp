#include "core/gpu_texture.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

GPUTexture::GPUTexture(const TextureInfo& info)
    : _info(info)
{
    auto& ctx = *RendService::device_context();

    _handle = ctx.create_texture(info.width, info.height, info.depth, info.mips, info.layers, info.format, info.samples, info.usage);
}

GPUTexture::~GPUTexture(void)
{
    auto& ctx = *RendService::device_context();

    ctx.destroy_texture(_handle);
}
