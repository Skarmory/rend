#ifndef REND_CORE_GPU_BUFFER_H
#define REND_CORE_GPU_BUFFER_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>

namespace rend
{

struct BufferInfo
{
    uint32_t    element_count{ 0 };
    size_t      element_size{ 0 };
    BufferUsage usage{ BufferUsage::NONE };
};

class GPUBuffer : public GPUResource, public RendObject
{
public:
    GPUBuffer(const std::string& name, RendHandle rend_handle, const BufferInfo& info);
    virtual ~GPUBuffer(void) = default;

    GPUBuffer(const GPUBuffer&)            = delete;
    GPUBuffer(GPUBuffer&&)                 = delete;
    GPUBuffer& operator=(const GPUBuffer&) = delete;
    GPUBuffer& operator=(GPUBuffer&&)      = delete;

    uint32_t        elements_count(void) const;
    size_t          element_size(void) const;
    BufferUsage     usage(void) const;
    size_t          bytes(void) const;

private:
    BufferInfo      _buffer_info{};
};

}

#endif
