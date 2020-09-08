#include "gpu_texture_base.h"

#include "device_context.h"

using namespace rend;

GPUTextureBase::~GPUTextureBase(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_texture(_handle);
}
