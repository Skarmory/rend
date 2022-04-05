#ifndef REND_GPU_TEXTURE_H
#define REND_GPU_TEXTURE_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"

#include <cstdint>

namespace rend
{

struct TextureInfo
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t depth{ 0 };
    uint32_t mips{ 0 };
    uint32_t layers{ 0 };
    Format format{ Format::R8G8B8A8 };
    ImageLayout layout{ ImageLayout::UNDEFINED };
    MSAASamples samples{ MSAASamples::MSAA_1X };
    ImageUsage usage{ ImageUsage::NONE };
};

class GPUTexture : public GPUResource
{
public:
    GPUTexture(const TextureInfo& info);
    ~GPUTexture(void);

    GPUTexture(const GPUTexture&)            = delete;
    GPUTexture(GPUTexture&&)                 = delete;
    GPUTexture& operator=(const GPUTexture&) = delete;
    GPUTexture& operator=(GPUTexture&&)      = delete;

    TextureHandle handle(void) const { return _handle; }

    uint32_t    width(void) const   { return _info.width; }
    uint32_t    height(void) const  { return _info.height; }
    uint32_t    depth(void) const   { return _info.depth; }
    uint32_t    mips(void) const    { return _info.mips; }
    uint32_t    layers(void) const  { return _info.layers; }
    Format      format(void) const  { return _info.format; }
    ImageLayout layout(void) const  { return _info.layout; }
    void        layout(ImageLayout layout) { _info.layout = layout; }
    MSAASamples samples(void) const { return _info.samples; }
    ImageUsage  usage(void) const  { return  _info.usage; }

protected:
    TextureHandle _handle{ NULL_HANDLE };
    TextureInfo   _info{};
};

}

#endif
