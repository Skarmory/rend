#ifndef REND_GPU_BUFFER_H
#define REND_GPU_BUFFER_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"

#include <string>

namespace rend
{

struct BufferInfo
{
    uint32_t    element_count{ 0 };
    size_t      element_size{ 0 };
    BufferUsage usage{ BufferUsage::NONE };
};

class GPUBuffer : public GPUResource
{
public:
    GPUBuffer(const BufferInfo& info);
    GPUBuffer(const std::string& name, const BufferInfo& info);
    ~GPUBuffer(void);

    GPUBuffer(const GPUBuffer&)            = delete;
    GPUBuffer(GPUBuffer&&)                 = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer& operator=(GPUBuffer&&)      = delete;

    BufferHandle handle(void) const { return _handle; }

    uint32_t elements_count(void) const { return _info.element_count; }
    size_t element_size(void) const { return _info.element_size; }
    BufferUsage usage(void) const { return _info.usage; }
    size_t bytes(void) const { return _bytes; }

private:
    BufferHandle _handle{ NULL_HANDLE };
    BufferInfo   _info{};
    size_t       _bytes;
};

}

#endif
