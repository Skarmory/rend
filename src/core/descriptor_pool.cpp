#include "core/descriptor_pool.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

DescriptorPool::DescriptorPool(const DescriptorPoolInfo& info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_descriptor_pool(info);
}

DescriptorPool::~DescriptorPool(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_descriptor_pool(_handle);
}

DescriptorPoolHandle DescriptorPool::handle(void) const
{
    return _handle;
}
