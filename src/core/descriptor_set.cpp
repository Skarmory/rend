#include "core/descriptor_set.h"

#include "core/device_context.h"

using namespace rend;

bool DescriptorSet::create(const DescriptorSetInfo& info)
{
    auto& ctx = DeviceContext::instance();

    DescriptorSetHandle handle = ctx.create_descriptor_set(info);

    if(handle == NULL_HANDLE)
    {
        return false;
    }

    _handle = handle;

    return true;
}

void DescriptorSet::destroy(void)
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
