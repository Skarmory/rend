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
    // Destroy descriptor sets now, before we destroy the VkDescriptorPool
    _descriptor_sets.clear();

    auto& ctx = *RendService::device_context();
    ctx.destroy_descriptor_pool(_handle);
}

DescriptorPoolHandle DescriptorPool::handle(void) const
{
    return _handle;
}

DescriptorSetHandle DescriptorPool::allocate_descriptor_set(const DescriptorSetInfo& info)
{
    return _descriptor_sets.allocate(_handle, info);
}

void DescriptorPool::dealloacte_descriptor_set(DescriptorSetHandle handle)
{
    _descriptor_sets.deallocate(handle);
}

DescriptorSet* DescriptorPool::get_descriptor_set(DescriptorSetHandle handle) const
{
    return _descriptor_sets.get(handle);
}
