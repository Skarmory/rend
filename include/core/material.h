#ifndef REND_CORE_MATERIAL_H
#define REND_CORE_MATERIAL_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>

namespace rend
{

class DescriptorSet;
class GPUTexture;
class RenderPass;

enum class MaterialBindingSlot
{
    ALBEDO
};

struct MaterialInfo
{
    GPUTexture* albedo_texture{ nullptr };
    RenderPass* render_pass{ nullptr };
};

class Material : public GPUResource, public RendObject
{
    public:
        Material(const std::string& name, const MaterialInfo& info, DescriptorSet* descriptor_set, RendHandle rend_handle);
        ~Material(void) = default;

        const MaterialInfo&  get_material_info(void) const;
        const DescriptorSet& get_descriptor_set(void) const;

    private:
        MaterialInfo _info{};
        DescriptorSet* _descriptor_set{ nullptr };
};

}

#endif
