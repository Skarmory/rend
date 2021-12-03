#include "core/descriptor_set_layout.h"

#include "core/device_context.h"

using namespace rend;

bool DescriptorSetLayout::create(const DescriptorSetLayoutInfo& info)
{
    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_descriptor_set_layout(info);

    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    return true;
}

void DescriptorSetLayout::destroy(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_descriptor_set_layout(_handle);
    _handle = NULL_HANDLE;
}

DescriptorSetLayoutHandle DescriptorSetLayout::handle(void) const
{
    return _handle;
}
