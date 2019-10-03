#include "vulkan_vertex_buffer.h"

#include "vulkan_gpu_buffer.h"

using namespace rend;

VulkanVertexBuffer::VulkanVertexBuffer(DeviceContext* context)
    : _context(context), _buffer(nullptr)
{
}

VulkanVertexBuffer::~VulkanVertexBuffer(void)
{
    delete _buffer;
}

VulkanGPUBuffer* VulkanVertexBuffer::gpu_buffer(void) const
{
    return _buffer;
}

bool VulkanVertexBuffer::create_vertex_buffer_api(uint32_t vertices_count, size_t vertex_size)
{
    _count = vertices_count;
    _bytes = vertices_count * vertex_size;
    _vertex_bytes = vertex_size;
    _buffer = new VulkanGPUBuffer(_context);
    return _buffer->create_buffer(_bytes, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}
