#ifndef REND_CORE_MATERIAL_H
#define REND_CORE_MATERIAL_H

#include "core/descriptor_set_binding.h"
#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

class DescriptorSet;
class DescriptorSetLayout;
class RenderStrategy;

struct MaterialInfo
{
    DescriptorSetLayout* descriptor_set_layout{ nullptr };
    RenderStrategy* render_strategy{ nullptr };
    std::vector<DescriptorSetBinding> descriptors;
};

class Material : public GPUResource, public RendObject
{
    public:
        Material(const std::string& name, const MaterialInfo& info);
        ~Material(void) = default;

        MaterialInfo&  get_material_info(void);
        DescriptorSet& get_descriptor_set(void);
        RenderStrategy& get_render_strategy(void);

    private:
        MaterialInfo _info{};
        DescriptorSet* _descriptor_set{ nullptr };
};

}

#endif
