#ifndef REND_VULKAN_UNIFORM_BUFFER_H
#define REND_VULKAN_UNIFORM_BUFFER_H

#include "uniform_buffer_base.h"

namespace rend
{

class DeviceContext;
class GPUBuffer;

class VulkanUniformBuffer : public UniformBufferBase
{
public:
    VulkanUniformBuffer(DeviceContext* context);
    virtual ~VulkanUniformBuffer(void);

    VulkanUniformBuffer(const VulkanUniformBuffer&) = delete;
    VulkanUniformBuffer(VulkanUniformBuffer&&) = delete;

    const VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = delete;
    const VulkanUniformBuffer& operator=(VulkanUniformBuffer&&) = delete;

    GPUBuffer* gpu_buffer(void) const;

protected:
    bool create_uniform_buffer_api(size_t bytes);

private:
    DeviceContext* _context;
    GPUBuffer* _buffer;
};

}

#endif
