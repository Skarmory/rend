#include "core/material.h"

#include "core/descriptor_set.h"

using namespace rend;

Material::Material(const std::string& name, const MaterialInfo& info, DescriptorSet* descriptor_set, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _info(info),
        _descriptor_set(descriptor_set)
{
}

const MaterialInfo& Material::get_material_info(void) const
{
    return _info;
}

const DescriptorSet& Material::get_descriptor_set(void) const
{
    return *_descriptor_set;
}
