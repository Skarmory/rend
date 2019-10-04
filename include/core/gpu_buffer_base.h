#ifndef REND_GPU_BUFFER_BASE_H
#define REND_GPU_BUFFER_BASE_H

#include "gpu_resource.h"

#include <cstddef>
#include <cstdint>

namespace rend
{

class GPUBufferBase : public GPUResource
{
public:
    GPUBufferBase(void);
    virtual ~GPUBufferBase(void);

    GPUBufferBase(const GPUBufferBase&)            = delete;
    GPUBufferBase(GPUBufferBase&&)                 = delete;
    GPUBufferBase& operator=(const GPUBufferBase&) = delete;
    GPUBufferBase& operator=(GPUBufferBase&&)      = delete;
};

}

#endif
