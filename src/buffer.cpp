#include "buffer.h"

#include "device_context.h"
#include "logical_device.h"
#include "gpu_buffer.h"

using namespace rend;

Buffer::Buffer(DeviceContext* context, size_t size_bytes, BufferType buffer_type)
    : _context(context), _type(buffer_type)
{
    VkMemoryPropertyFlags memory_properties;
    VkBufferUsageFlags    buffer_usage;

    switch(_type)
    {
        case BufferType::VERTEX:
            memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            buffer_usage      = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            break;
        case BufferType::INDEX:
            memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            buffer_usage      = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            break;
        case BufferType::UNIFORM:
            memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            buffer_usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        case BufferType::STAGING:
            memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            buffer_usage      = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            break;
    }

    _gpu_buffer = _context->get_device()->create_buffer(size_bytes, memory_properties, buffer_usage);
}

Buffer::~Buffer(void)
{
    _context->get_device()->destroy_buffer(&_gpu_buffer);
}

GPUBuffer* Buffer::get_gpu_buffer(void) const
{
    return _gpu_buffer;
}
