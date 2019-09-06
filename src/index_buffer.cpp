#include "index_buffer.h"

using namespace rend;

#ifdef USE_VULKAN
IndexBuffer::IndexBuffer(DeviceContext* context) : VulkanIndexBuffer(context)
#else
IndexBuffer::IndexBuffer(DeviceContext* context)
#endif
{
}

IndexBuffer::~IndexBuffer(void)
{
}

bool IndexBuffer::create_index_buffer(/*void* data,*/ uint32_t indices_count, size_t index_size)
{
    if(!create_index_buffer_api(/*data,*/ indices_count, index_size))
    {
        // TODO: Log stuff
        return false;
    }

    return true;
}
