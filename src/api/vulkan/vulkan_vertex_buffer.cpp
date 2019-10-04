#include "vulkan_vertex_buffer.h"

#include "vulkan_gpu_buffer.h"

using namespace rend;

VulkanVertexBuffer::VulkanVertexBuffer(DeviceContext& context)
    : VulkanGPUBuffer(context)
{
}

VulkanVertexBuffer::~VulkanVertexBuffer(void)
{
}

StatusCode VulkanVertexBuffer::create_vertex_buffer_api(uint32_t vertices_count, size_t vertex_size)
{
    StatusCode code = create_buffer(vertices_count * vertex_size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    if(code != StatusCode::SUCCESS)
    {
        _count = vertices_count;
        _vertex_bytes = vertex_size;
    }

    return code;
}
