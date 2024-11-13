#include "core/gpu_buffer.h"

#include "core/renderer.h"
#include "core/logging/log_defs.h"
#include "core/logging/log_helper_funcs.h"
#include "core/logging/log_manager.h"

using namespace rend;

GPUBuffer::GPUBuffer(const std::string& name, const BufferInfo& info)
    :
        GPUResource(name),
        _buffer_info(info)
{
    _data = (char*)malloc(info.element_count * info.element_size);

#ifdef DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "BUFFER | Create buffer with params: " + rend::core::logging::to_string(_buffer_info));
#endif
}

GPUBuffer::~GPUBuffer(void)
{
    free(_data);
}

uint32_t GPUBuffer::elements_count(void) const
{
    return _buffer_info.element_count;
}

size_t GPUBuffer::element_size(void) const
{
    return _buffer_info.element_size;
}

void* GPUBuffer::data(void)
{
    return _data;
}

BufferUsage GPUBuffer::usage(void) const
{
    return _buffer_info.usage;
}

size_t GPUBuffer::bytes(void) const
{
    return _buffer_info.element_size * _buffer_info.element_count;
}

void GPUBuffer::store_data(char* data, size_t size_bytes)
{
    memcpy(_data, data, size_bytes);
}

void GPUBuffer::load_to_gpu(void)
{
    auto& rr = Renderer::get_instance();
    //rr.load_buffer(*this, _data.data(), _data.size(), 0);
    rr.load_buffer(*this);
}
