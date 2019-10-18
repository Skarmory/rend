#ifndef REND_UNIFORM_BUFFER_H
#define REND_UNIFORM_BUFFER_H

#include <cstdint>
#include <cstddef>

#ifdef USE_VULKAN
#include "vulkan_uniform_buffer.h"
#endif

namespace rend
{

#ifdef USE_VULKAN
class UniformBuffer : public VulkanUniformBuffer
#elif
class UniformBuffer
#endif
{
public:
    explicit UniformBuffer(DeviceContext& context);
    ~UniformBuffer(void);

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&&) = delete;
    const UniformBuffer& operator=(const UniformBuffer&) = delete;
    const UniformBuffer& operator=(UniformBuffer&&) = delete;

    bool create_uniform_buffer(size_t bytes);
};

}

#endif
