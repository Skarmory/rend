#ifndef REND_VULKAN_UNIFORM_BUFFER_H
#define REND_VULKAN_UNIFORM_BUFFER_H

#include "uniform_buffer_base.h"
#include "vulkan_gpu_buffer.h"
#include "rend_defs.h"

namespace rend
{

class VulkanUniformBuffer : public VulkanGPUBuffer, public UniformBufferBase
{
public:
    VulkanUniformBuffer(void) = default;
    ~VulkanUniformBuffer(void) = default;

    VulkanUniformBuffer(const VulkanUniformBuffer&)            = delete;
    VulkanUniformBuffer(VulkanUniformBuffer&&)                 = delete;
    VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = delete;
    VulkanUniformBuffer& operator=(VulkanUniformBuffer&&)      = delete;

protected:
    StatusCode create_uniform_buffer_api(size_t bytes);
};

}

#endif
