#ifndef REND_CORE_GPU_TEXTURE_H
#define REND_CORE_GPU_TEXTURE_H

#include "core/gpu_resource.h"
#include "core/i_loadable.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"
#include "core/texture_info.h"

#include <cstdint>
#include <string>
#include <vector>

namespace rend
{

class GPUTexture : public GPUResource, public RendObject, public ILoadable
{
public:
    GPUTexture(const std::string& name, const TextureInfo& info);
    virtual ~GPUTexture(void);

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

    const TextureInfo& get_info(void) const;

    void* data(void);
    uint32_t bytes(void) const;

    void store_data(char* data, size_t size_bytes) override;
    void load_to_gpu(void) override;

protected:
    TextureInfo _info{};
    char* _data{ nullptr };
};

}

#endif
