#include "vulkan_uniform_buffer.h"

#include "device_context.h"
#include "gpu_buffer.h"

using namespace rend;

VulkanUniformBuffer::VulkanUniformBuffer(DeviceContext* context)
    : _context(context), _buffer(nullptr)
{
}

VulkanUniformBuffer::~VulkanUniformBuffer(void)
{
    delete _buffer;
}

GPUBuffer* VulkanUniformBuffer::gpu_buffer(void) const
{
    return _buffer;
}

bool VulkanUniformBuffer::create_uniform_buffer_api(size_t bytes)
{
    _bytes = bytes;
    _buffer = new GPUBuffer(_context); 
    return _buffer->create(_bytes, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}
