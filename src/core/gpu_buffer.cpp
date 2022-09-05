#include "core/gpu_buffer.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

namespace
{
    static uint32_t _unnamed_buffer_count = 0;
}

GPUBuffer::GPUBuffer(const BufferInfo& info)
    :
        GPUBuffer("Unnamed buffer " + std::to_string(::_unnamed_buffer_count++), info)
{
}

GPUBuffer::GPUBuffer(const std::string& name, const BufferInfo& info)
    : GPUResource(name),
      _info(info)
{
    auto& ctx = *RendService::device_context();

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
    auto& ctx = *RendService::device_context();

    ctx.destroy_buffer(_handle);
}
