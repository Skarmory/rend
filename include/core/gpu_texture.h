#ifndef REND_CORE_GPU_TEXTURE_H
#define REND_CORE_GPU_TEXTURE_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <cstdint>
#include <string>

namespace rend
{

struct TextureInfo
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t depth{ 0 };
    SizeRatio size_ratio{}; // Overrides width, height, sets to some ratio of the full screen
    uint32_t mips{ 0 };
    uint32_t layers{ 0 };
    Format format{ Format::R8G8B8A8 };
    ImageLayout layout{ ImageLayout::UNDEFINED };
    MSAASamples samples{ MSAASamples::MSAA_1X };
    ImageUsage usage{ ImageUsage::NONE };
};

class GPUTexture : public GPUResource, public RendObject
{
public:
    GPUTexture(const std::string& name, const TextureInfo& info, RendHandle rend_handle);
    virtual ~GPUTexture(void) = default;

    GPUTexture(const GPUTexture&)            = delete;
    GPUTexture(GPUTexture&&)                 = delete;
    GPUTexture& operator=(const GPUTexture&) = delete;
    GPUTexture& operator=(GPUTexture&&)      = delete;

    uint32_t    width(void) const;
    uint32_t    height(void) const;
    uint32_t    depth(void) const;
    uint32_t    mips(void) const;
    uint32_t    layers(void) const;
    Format      format(void) const;
    ImageLayout layout(void) const;
    void        layout(ImageLayout layout);
    MSAASamples samples(void) const;
    ImageUsage  usage(void) const;

protected:
    TextureInfo     _info{};
};

}

#endif
