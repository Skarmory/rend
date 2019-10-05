#include "gpu_texture_base.h"

using namespace rend;

GPUTextureBase::GPUTextureBase(void)
    : _width(0),
      _height(0),
      _depth(0)
{
}

GPUTextureBase::~GPUTextureBase(void)
{
}

uint32_t GPUTextureBase::width(void) const
{
    return _width;
}

uint32_t GPUTextureBase::height(void) const
{
    return _height;
}

uint32_t GPUTextureBase::depth(void) const
{
    return _depth;
}
