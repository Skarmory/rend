#include "core/descriptor_set.h"

#include "core/device_context.h"

using namespace rend;

DescriptorSet::DescriptorSet(const DescriptorSetInfo& info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_descriptor_set(info);
}

DescriptorSet::~DescriptorSet(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_descriptor_set(_handle);
}

void DescriptorSet::add_binding(const rend::DescriptorSetBinding& binding) const
{
   auto& ctx = DeviceContext::instance();

   ctx.add_descriptor_binding(_handle, binding);
}

DescriptorSetHandle DescriptorSet::handle(void) const
{
    return _handle;
}
