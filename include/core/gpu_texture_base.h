#ifndef REND_GPU_TEXTURE_BASE_H
#define REND_GPU_TEXTURE_BASE_H

#include "gpu_resource.h"
#include "rend_defs.h"

#include <cstdint>

namespace rend
{

class GPUTextureBase : public GPUResource
{
public:
    GPUTextureBase(void) = default;
    ~GPUTextureBase(void);

    GPUTextureBase(const GPUTextureBase&)            = delete;
    GPUTextureBase(GPUTextureBase&&)                 = delete;
    GPUTextureBase& operator=(const GPUTextureBase&) = delete;
    GPUTextureBase& operator=(GPUTextureBase&&)      = delete;

    TextureHandle get_handle(void) const { return _handle; }

    uint32_t    width(void) const   { return _width; }
    uint32_t    height(void) const  { return _height; }
    uint32_t    depth(void) const   { return _depth; }
    uint32_t    layers(void) const  { return _layers; }
    Format      format(void) const  { return _format; }
    ImageLayout layout(void) const  { return _layout; }
    void        layout(ImageLayout layout) { _layout = layout; }
    MSAASamples samples(void) const { return _samples; }

protected:
    TextureHandle _handle{ NULL_HANDLE };
    uint32_t      _width{ 0 };
    uint32_t      _height{ 0 };
    uint32_t      _depth{ 0 };
    uint32_t      _layers{ 0 };
    Format        _format{ rend::Format::B8G8R8A8 };
    ImageLayout   _layout{ rend::ImageLayout::UNDEFINED };
    MSAASamples   _samples{ MSAASamples::MSAA_1X };
};

}

#endif
