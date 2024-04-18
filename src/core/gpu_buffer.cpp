#include "core/gpu_buffer.h"

using namespace rend;

GPUBuffer::GPUBuffer(const std::string& name, RendHandle rend_handle, const BufferInfo& info)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _buffer_info(info)
{
}

uint32_t GPUBuffer::elements_count(void) const
{
    return _buffer_info.element_count;
}

size_t GPUBuffer::element_size(void) const
{
    return _buffer_info.element_size;
}

BufferUsage GPUBuffer::usage(void) const
{
    return _buffer_info.usage;
}

size_t GPUBuffer::bytes(void) const
{
    return _buffer_info.element_size * _buffer_info.element_count;
}
