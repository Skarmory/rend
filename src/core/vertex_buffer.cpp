#include "vertex_buffer.h"

#include "rend_defs.h"

using namespace rend;

#ifdef USE_VULKAN
VertexBuffer::VertexBuffer(DeviceContext& context) : VulkanVertexBuffer(context)
#elif
VertexBuffer::VertexBuffer(DeviceContext& context)
#endif
{
}

VertexBuffer::~VertexBuffer(void)
{
}

bool VertexBuffer::create_vertex_buffer(uint32_t vertices_count, size_t vertex_size)
{
    if(create_vertex_buffer_api(vertices_count, vertex_size) != StatusCode::SUCCESS)
    {
        // TODO: Log stuff
        return false;
    }

    _count = vertices_count;
    _vertex_bytes = vertex_size;

    return true;
}
