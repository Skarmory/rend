#include "gpu_buffer.h"

#include "device_context.h"

using namespace rend;

GPUBuffer::~GPUBuffer(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_buffer(_handle);
}

bool GPUBuffer::create(const BufferInfo& info)
{
    auto& ctx = DeviceContext::instance();

    //TODO: This seems weird. Pushback GPUBuffer and GPUTexture creation and storage to the device contexts?
    if((info.usage & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE)
    {
        _handle = ctx.create_vertex_buffer(info.element_count, info.element_count);
    }
    else if((info.usage & BufferUsage::INDEX_BUFFER) != BufferUsage::NONE)
    {
        _handle = ctx.create_index_buffer(info.element_count, info.element_size);
    }
    else if((info.usage & BufferUsage::UNIFORM_BUFFER) != BufferUsage::NONE)
    {
        _handle = ctx.create_uniform_buffer(info.element_size * info.element_count);
    }

    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    _bytes = info.element_size * info.element_count;

    return true;
}
