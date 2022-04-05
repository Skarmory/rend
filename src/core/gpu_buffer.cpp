#include "core/gpu_buffer.h"

#include "core/device_context.h"

using namespace rend;

GPUBuffer::GPUBuffer(const BufferInfo& info)
    : _info(info)
{
    auto& ctx = DeviceContext::instance();

    //TODO: This seems weird. Pushback GPUBuffer and GPUTexture creation and storage to the device contexts?
    if((info.usage & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE)
    {
        _handle = ctx.create_vertex_buffer(info.element_count, info.element_size);
    }
    else if((info.usage & BufferUsage::INDEX_BUFFER) != BufferUsage::NONE)
    {
        _handle = ctx.create_index_buffer(info.element_count, info.element_size);
    }
    else if((info.usage & BufferUsage::UNIFORM_BUFFER) != BufferUsage::NONE)
    {
        _handle = ctx.create_uniform_buffer(info.element_count * info.element_size);
    }

    _bytes = info.element_size * info.element_count;
}

GPUBuffer::~GPUBuffer(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_buffer(_handle);
}
