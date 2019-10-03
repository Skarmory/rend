#include "vulkan_index_buffer.h"

#include "device_context.h"
#include "vulkan_gpu_buffer.h"
#include "logical_device.h"

using namespace rend;

VulkanIndexBuffer::VulkanIndexBuffer(DeviceContext* context)
    : _context(context),
      _buffer(nullptr)
{
}

VulkanIndexBuffer::~VulkanIndexBuffer(void)
{
    delete _buffer;
}

VulkanGPUBuffer* VulkanIndexBuffer::gpu_buffer(void) const
{
    return _buffer;
}

bool VulkanIndexBuffer::create_index_buffer_api(uint32_t indices_count, size_t index_size)
{
    _count = indices_count;
    _bytes = indices_count * index_size;
    _buffer = new VulkanGPUBuffer(_context);
    return _buffer->create_buffer(_bytes, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}
