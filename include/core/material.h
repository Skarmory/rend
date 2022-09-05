#ifndef REND_CORE_MATERIAL_H
#define REND_CORE_MATERIAL_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"

#include <string>
#include <vector>

namespace rend
{

struct MaterialInfo
{
    TextureHandle albedo_texture_h{ NULL_HANDLE };
    RenderPassHandle render_pass_h{ NULL_HANDLE };
};

class Material : public GPUResource
{
    public:
        Material(const std::string& name, const MaterialInfo& info);
        ~Material(void);

        const MaterialInfo& get_material_info(void) const;
        DescriptorSetHandle get_descriptor_set(void) const;

    private:
        MaterialInfo _info{};
        DescriptorSetHandle _descriptor_set_h{ NULL_HANDLE };
};

}

#endif
