#ifndef REND_UNIFORM_BUFFER_H
#define REND_UNIFORM_BUFFER_H

#include <cstdint>
#include <cstddef>

#include "gpu_buffer_base.h"
#include "rend_defs.h"

namespace rend
{

class UniformBuffer : public GPUBufferBase
{
public:
    UniformBuffer(void) = default;
    ~UniformBuffer(void) = default;

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&&) = delete;
    const UniformBuffer& operator=(const UniformBuffer&) = delete;
    const UniformBuffer& operator=(UniformBuffer&&) = delete;

    bool create_uniform_buffer(size_t bytes);
};

}

#endif
