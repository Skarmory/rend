#ifndef REND_GPU_TEXTURE_H
#define REND_GPU_TEXTURE_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"

#include <cstdint>

namespace rend
{

struct TextureInfo
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mips;
    uint32_t layers;
    Format format;
    ImageLayout layout;
    MSAASamples samples;
    ImageUsage usage;
};

class GPUTexture : public GPUResource
{
public:
    GPUTexture(void) = default;
    ~GPUTexture(void) = default;

    GPUTexture(const GPUTexture&)            = delete;
    GPUTexture(GPUTexture&&)                 = delete;
    GPUTexture& operator=(const GPUTexture&) = delete;
    GPUTexture& operator=(GPUTexture&&)      = delete;

    bool create(const TextureInfo& info);
    void destroy(void);

    TextureHandle get_handle(void) const { return _handle; }

    uint32_t    width(void) const   { return _width; }
    uint32_t    height(void) const  { return _height; }
    uint32_t    depth(void) const   { return _depth; }
    uint32_t    mips(void) const    { return _mips; }
    uint32_t    layers(void) const  { return _layers; }
    Format      format(void) const  { return _format; }
    ImageLayout layout(void) const  { return _layout; }
    void        layout(ImageLayout layout) { _layout = layout; }
    MSAASamples samples(void) const { return _samples; }
    ImageUsage  usage(void) const  { return  _usage; }

protected:
    TextureHandle _handle{ NULL_HANDLE };
    uint32_t      _width{ 0 };
    uint32_t      _height{ 0 };
    uint32_t      _depth{ 0 };
    uint32_t      _mips{ 0 };
    uint32_t      _layers{ 0 };
    Format        _format{ Format::B8G8R8A8 };
    ImageLayout   _layout{ ImageLayout::UNDEFINED };
    MSAASamples   _samples{ MSAASamples::MSAA_1X };
    ImageUsage    _usage{ ImageUsage::NONE };
};

}

#endif
