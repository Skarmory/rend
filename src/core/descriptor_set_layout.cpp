#include "core/descriptor_set_layout.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutInfo& info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_descriptor_set_layout(info);
}

DescriptorSetLayout::~DescriptorSetLayout(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_descriptor_set_layout(_handle);
}

DescriptorSetLayoutHandle DescriptorSetLayout::handle(void) const
{
    return _handle;
}
