#include "core/gpu_texture.h"

#include "core/renderer.h"

#include <cassert>

using namespace rend;

GPUTexture::GPUTexture(const std::string& name, const TextureInfo& info)
    :
      GPUResource(name),
      _info(info)
{
    _data = (char*)malloc(bytes());
}

GPUTexture::~GPUTexture(void)
{
    free(_data);
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

const TextureInfo& GPUTexture::get_info(void) const
{
    return _info;
}

void* GPUTexture::data(void)
{
    return _data;
}

uint32_t GPUTexture::bytes(void) const
{
    return (_info.width * _info.height * (_info.depth == 0 ? 1 : _info.depth) * _info.layers) * 4;
}

void GPUTexture::store_data(char* data, size_t size_bytes)
{
    assert(size_bytes <= bytes() && "GPUTexture, attempt to store data larger than texture buffer");
    memcpy(_data, data, size_bytes);
}

void GPUTexture::load_to_gpu(void)
{
    auto& rr = rend::Renderer::get_instance();
    rr.load_texture(*this);
}
