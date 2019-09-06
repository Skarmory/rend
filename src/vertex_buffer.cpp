#include "vertex_buffer.h"

using namespace rend;

#ifdef USE_VULKAN
VertexBuffer::VertexBuffer(DeviceContext* context) : VulkanVertexBuffer(context)
#elif
VertexBuffer::VertexBuffer(DeviceContext* context)
#endif
{
}

VertexBuffer::~VertexBuffer(void)
{
}

bool VertexBuffer::create_vertex_buffer(uint32_t indices_count, size_t index_size)
{
    if(!create_vertex_buffer_api(indices_count, index_size))
    {
        // TODO: Log stuff
        return false;
    }

    return true;
}
