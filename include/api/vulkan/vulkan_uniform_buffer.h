#ifndef REND_VULKAN_UNIFORM_BUFFER_H
#define REND_VULKAN_UNIFORM_BUFFER_H

#include "uniform_buffer_base.h"

namespace rend
{

class DeviceContext;
class VulkanGPUBuffer;

class VulkanUniformBuffer : public UniformBufferBase
{
public:
    VulkanUniformBuffer(DeviceContext* context);
    virtual ~VulkanUniformBuffer(void);

    VulkanUniformBuffer(const VulkanUniformBuffer&)            = delete;
    VulkanUniformBuffer(VulkanUniformBuffer&&)                 = delete;
    VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = delete;
    VulkanUniformBuffer& operator=(VulkanUniformBuffer&&)      = delete;

    VulkanGPUBuffer* gpu_buffer(void) const;

protected:
    bool create_uniform_buffer_api(size_t bytes);

private:
    DeviceContext* _context;
    VulkanGPUBuffer* _buffer;
};

}

#endif
