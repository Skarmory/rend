#include "uniform_buffer.h"

#include "device_context.h"

using namespace rend;

bool UniformBuffer::create_uniform_buffer(size_t bytes)
{
    auto& ctx = DeviceContext::instance();

    _handle = ctx.create_uniform_buffer(bytes);

    if (_handle != NULL_HANDLE)
    {
        _bytes = bytes;

        return true;
    }

    return false;
}
