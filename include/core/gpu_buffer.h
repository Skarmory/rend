#ifndef REND_GPU_BUFFER_H
#define REND_GPU_BUFFER_H

#include "gpu_resource.h"
#include "rend_defs.h"

namespace rend
{

struct BufferInfo
{
    size_t      element_count;
    size_t      element_size;
    BufferUsage usage;
};

class GPUBuffer : public GPUResource
{
public:
    GPUBuffer(void) = default;
    ~GPUBuffer(void) = default;

    GPUBuffer(const GPUBuffer&)            = delete;
    GPUBuffer(GPUBuffer&&)                 = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer& operator=(GPUBuffer&&)      = delete;

    bool create(const BufferInfo& info);
    void destroy(void);

    BufferHandle get_handle(void) const { return _handle; }

private:
    BufferHandle _handle{ NULL_HANDLE };
};

}

#endif
