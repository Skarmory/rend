#include "core/descriptor_set.h"

#include "core/device_context.h"
#include "core/renderer.h"
#include "core/rend_service.h"

using namespace rend;

DescriptorSet::DescriptorSet(DescriptorPoolHandle pool_h, const DescriptorSetInfo& info)
    :
        _set_number(info.set_number)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_descriptor_set(pool_h, info);
}

DescriptorSet::~DescriptorSet(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_descriptor_set(_handle);
}

void DescriptorSet::add_texture_binding(uint32_t slot, TextureHandle handle)
{
    auto* rr = RendService::renderer();
    _bindings.emplace_back(
        slot, DescriptorType::COMBINED_IMAGE_SAMPLER, rr->get_texture(handle)->handle()
    );
}

void DescriptorSet::add_uniform_buffer_binding(uint32_t slot, BufferHandle handle)
{
    auto* rr = RendService::renderer();
    _bindings.emplace_back(
        slot, DescriptorType::UNIFORM_BUFFER, rr->get_buffer(handle)->handle()
    );
}

void DescriptorSet::write_bindings(void)
{
   auto& ctx = *RendService::device_context();
   ctx.write_descriptor_bindings(_handle, _bindings);
}

DescriptorSetHandle DescriptorSet::handle(void) const
{
    return _handle;
}

uint32_t DescriptorSet::set_number(void) const
{
    return _set_number;
}
