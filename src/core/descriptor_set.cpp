#include "core/descriptor_set.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

DescriptorSet::DescriptorSet(const DescriptorSetInfo& info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_descriptor_set(info);
}

DescriptorSet::~DescriptorSet(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_descriptor_set(_handle);
}

void DescriptorSet::add_binding(const rend::DescriptorSetBinding& binding) const
{
   auto& ctx = *RendService::device_context();

   ctx.add_descriptor_binding(_handle, binding);
}

DescriptorSetHandle DescriptorSet::handle(void) const
{
    return _handle;
}
