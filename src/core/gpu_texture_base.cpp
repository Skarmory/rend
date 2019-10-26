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

bool GPUTextureBase::create_texture_base(uint32_t width, uint32_t height, uint32_t depth, Format format)
{
    _width  = width;
    _height = height;
    _depth  = depth;
    _format = format;

    return true;
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

Format GPUTextureBase::format(void) const
{
    return _format;
}
