#ifndef REND_CORE_GPU_BUFFER_H
#define REND_CORE_GPU_BUFFER_H

#include "core/gpu_resource.h"
#include "core/i_loadable.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

class DescriptorSet;

struct BufferInfo
{
    uint32_t    element_count{ 0 };
    size_t      element_size{ 0 };
    BufferUsage usage{ BufferUsage::NONE };
};

class GPUBuffer : public GPUResource, public RendObject, public ILoadable
{
public:
    GPUBuffer(const std::string& name, const BufferInfo& info);
    virtual ~GPUBuffer(void);

    GPUBuffer(const GPUBuffer&)            = delete;
    GPUBuffer(GPUBuffer&&)                 = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer& operator=(GPUBuffer&&)      = delete;

    uint32_t        elements_count(void) const;
    size_t          element_size(void) const;
    void*           data(void);
    BufferUsage     usage(void) const;
    size_t          bytes(void) const;

    void store_data(char* data, size_t size_bytes) override;
    void load_to_gpu(void) override;

private:
    BufferInfo _buffer_info{};
    char* _data;
};

}

#endif
