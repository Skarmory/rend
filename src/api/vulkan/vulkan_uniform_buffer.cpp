#include "vulkan_uniform_buffer.h"

#include "device_context.h"
#include "vulkan_gpu_buffer.h"

using namespace rend;

VulkanUniformBuffer::VulkanUniformBuffer(DeviceContext* context)
    : _context(context), _buffer(nullptr)
{
}

VulkanUniformBuffer::~VulkanUniformBuffer(void)
{
    delete _buffer;
}

VulkanGPUBuffer* VulkanUniformBuffer::gpu_buffer(void) const
{
    return _buffer;
}

bool VulkanUniformBuffer::create_uniform_buffer_api(size_t bytes)
{
    _bytes = bytes;
    _buffer = new VulkanGPUBuffer(_context);
    return _buffer->create_buffer(_bytes, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}
