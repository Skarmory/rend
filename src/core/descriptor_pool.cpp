#include "core/descriptor_pool.h"

#include "core/device_context.h"

using namespace rend;

bool DescriptorPool::create(const DescriptorPoolInfo& info)
{
    auto& ctx = DeviceContext::instance();

    DescriptorPoolHandle handle = ctx.create_descriptor_pool(info);

    if(handle != NULL_HANDLE)
    {
        _handle = handle;
        return true;
    }

    return false;
}

void DescriptorPool::destroy(void)
{
    auto& ctx = DeviceContext::instance();

    ctx.destroy_descriptor_pool(_handle);

    _handle = NULL_HANDLE;
}

DescriptorPoolHandle DescriptorPool::handle(void) const
{
    return _handle;
}
