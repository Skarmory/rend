#include "core/gpu_texture.h"

using namespace rend;

GPUTexture::GPUTexture(const std::string& name, const TextureInfo& info, RendHandle rend_handle)
    :
      GPUResource(name),
      RendObject(rend_handle),
      _info(info)
{
}

uint32_t GPUTexture::width(void) const
{
    return _info.width;
}

uint32_t GPUTexture::height(void) const
{
    return _info.height;
}

uint32_t GPUTexture::depth(void) const
{
    return _info.depth;
}

uint32_t GPUTexture::mips(void) const
{
    return _info.mips;
}

uint32_t GPUTexture::layers(void) const
{
    return _info.layers;
}

Format GPUTexture::format(void) const
{
    return _info.format;
}

ImageLayout GPUTexture::layout(void) const
{
    return _info.layout;
}

void GPUTexture::layout(ImageLayout layout)
{
    _info.layout = layout;
}

MSAASamples GPUTexture::samples(void) const
{
    return _info.samples;
}

ImageUsage GPUTexture::usage(void) const
{
    return  _info.usage;
}
