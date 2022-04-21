#include "core/descriptor_pool.h"

#include "core/device_context.h"

using namespace rend;

DescriptorPool::DescriptorPool(const DescriptorPoolInfo& info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_descriptor_pool(info);
}

DescriptorPool::~DescriptorPool(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_descriptor_pool(_handle);
}

DescriptorPoolHandle DescriptorPool::handle(void) const
{
    return _handle;
}
