#include "core/material.h"

#include "core/descriptor_set.h"
#include "core/rend_service.h"

#include "api/vulkan/renderer.h"

using namespace rend;

namespace
{
    constexpr uint32_t ALBEDO_BINDING_SLOT = 0;
}

Material::Material(const std::string& name, const MaterialInfo& info)
    :
        GPUResource(name),
        _info(info)
{
    auto* rr = RendService::renderer();
    auto* rp = rr->get_render_pass(_info.render_pass_h);
    auto* sp = rr->get_sub_pass(rp->get_subpasses()[0]);
    auto* ss = rr->get_shader_set(sp->get_shader_set());

    _descriptor_set_h = rr->create_descriptor_set(ss->get_descriptor_set_layout(DescriptorFrequency::MATERIAL), DescriptorFrequency::MATERIAL);
    auto* ds = rr->get_descriptor_set(_descriptor_set_h);

    if(_info.albedo_texture_h != NULL_HANDLE)
    {
        ds->add_texture_binding(ALBEDO_BINDING_SLOT, _info.albedo_texture_h);
    }

    ds->write_bindings();
}

Material::~Material(void)
{
}

const MaterialInfo& Material::get_material_info(void) const
{
    return _info;
}

DescriptorSetHandle Material::get_descriptor_set(void) const
{
    return _descriptor_set_h;
}
