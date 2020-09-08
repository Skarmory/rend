#ifndef REND_GPU_BUFFER_BASE_H
#define REND_GPU_BUFFER_BASE_H

#include "gpu_resource.h"
#include "rend_defs.h"

namespace rend
{

class GPUBufferBase : public GPUResource
{
public:
    GPUBufferBase(void) = default;
    ~GPUBufferBase(void);

    GPUBufferBase(const GPUBufferBase&)            = delete;
    GPUBufferBase(GPUBufferBase&&)                 = delete;
    GPUBufferBase& operator=(const GPUBufferBase&) = delete;
    GPUBufferBase& operator=(GPUBufferBase&&)      = delete;

    BufferHandle get_handle(void) const { return _handle; }

protected:
    BufferHandle _handle{ NULL_HANDLE };
};

}

#endif
