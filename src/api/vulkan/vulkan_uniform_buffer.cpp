#include "vulkan_uniform_buffer.h"

#include "device_context.h"
#include "vulkan_gpu_buffer.h"

using namespace rend;

VulkanUniformBuffer::VulkanUniformBuffer(DeviceContext& context)
    : VulkanGPUBuffer(context)
{
}

VulkanUniformBuffer::~VulkanUniformBuffer(void)
{
}

StatusCode VulkanUniformBuffer::create_uniform_buffer_api(size_t bytes)
{
    StatusCode code = create_buffer(bytes, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    return code;
}
