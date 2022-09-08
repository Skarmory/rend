#include "core/gpu_texture.h"

#include "core/device_context.h"
#include "core/rend_service.h"
#include "core/renderer.h"

#include <functional>

using namespace rend;

namespace
{
    static uint32_t _unnamed_texture_count = 0;
}

GPUTexture::GPUTexture(const TextureInfo& info)
    :
        GPUTexture("Unnamed texture " + std::to_string(::_unnamed_texture_count++), info)
{
}

GPUTexture::GPUTexture(const std::string& name, const TextureInfo& info)
    :
        GPUResource(name),
        _info(info)
{
    auto& ctx = *RendService::device_context();
    auto& rr  = *RendService::renderer();

    if(_info.width == 0)
    {
        rr.get_size_by_ratio(_info.size_ratio, _info.width, _info.height);
    }

    _handle = ctx.create_texture(_info.width, _info.height, _info.depth, _info.mips, _info.layers, _info.format, _info.samples, _info.usage);
}

GPUTexture::GPUTexture(const std::string& name, const TextureInfo& info, TextureHandle handle)
    :
      GPUResource(name),
      _handle(handle),
      _info(info)
{
}

GPUTexture::~GPUTexture(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_texture(_handle);
}
