#ifndef REND_GPU_TEXTURE_BASE_H
#define REND_GPU_TEXTURE_BASE_H

#include "gpu_resource.h"

#include <cstdint>

namespace rend
{

enum class TextureFormat;

class GPUTextureBase : public GPUResource
{
public:
    GPUTextureBase(void);
    virtual ~GPUTextureBase(void);

    GPUTextureBase(const GPUTextureBase&)            = delete;
    GPUTextureBase(GPUTextureBase&&)                 = delete;
    GPUTextureBase& operator=(const GPUTextureBase&) = delete;
    GPUTextureBase& operator=(GPUTextureBase&&)      = delete;

    uint32_t      width(void) const;
    uint32_t      height(void) const;
    uint32_t      depth(void) const;
    TextureFormat format(void) const;

protected:
    uint32_t      _width;
    uint32_t      _height;
    uint32_t      _depth;
    TextureFormat _format;
};

}

#endif
