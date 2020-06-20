#include "index_buffer.h"

#include "device_context.h"

using namespace rend;

bool IndexBuffer::create_index_buffer(uint32_t indices_count, size_t index_size)
{
    auto& ctx = DeviceContext::instance();

    _handle = ctx.create_index_buffer(indices_count, index_size);

    if (_handle != NULL_HANDLE)
    {
        _bytes = indices_count * index_size;
    }

    return _handle;
}
