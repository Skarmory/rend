#include "index_buffer.h"

#include "rend_defs.h"

using namespace rend;

#ifdef USE_VULKAN
IndexBuffer::IndexBuffer(DeviceContext& context) : VulkanIndexBuffer(context)
#else
IndexBuffer::IndexBuffer(DeviceContext& context)
#endif
{
}

IndexBuffer::~IndexBuffer(void)
{
}

bool IndexBuffer::create_index_buffer(uint32_t indices_count, size_t index_size)
{
    if(create_index_buffer_api(indices_count, index_size) != StatusCode::SUCCESS)
    {
        // TODO: Log stuff
        return false;
    }

    _count = indices_count;

    return true;
}
