#include "gpu_buffer_base.h"

#include "device_context.h"

using namespace rend;

GPUBufferBase::~GPUBufferBase(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_buffer(_handle);
}
