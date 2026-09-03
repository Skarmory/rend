#ifndef REND_CORE_SHADER_SET_H
#define REND_CORE_SHADER_SET_H

#include "rend/rend_api.h"
#include "rend/core/descriptor_update_rate.h"
#include "rend/core/descriptor_set_binding.h"
#include "rend/core/descriptor_set_layout_binding.h"
#include "rend/core/rend_defs.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_object.h"

#include <array>
#include <string>
#include <vector>

namespace rend
{

class DescriptorSetLayout;
class PipelineLayout;
class Shader;

struct ShaderSetInfo
{
    std::array<const Shader*, SHADER_STAGE_COUNT> shaders{};
    std::vector<VertexBindingInfo> binding_info;
    std::array<DescriptorSetLayout*, (int)DescriptorUpdateRate::Count> layouts{ nullptr, nullptr, nullptr, nullptr, nullptr };
    std::vector<PushConstantRange> push_constant_ranges;

};

class REND_API ShaderSet : public GPUResource, public RendObject
{
public:
    ShaderSet(const std::string& name, const ShaderSetInfo& info, PipelineLayout* layout);
    ~ShaderSet(void);

    [[nodiscard]] const Shader* get_shader(ShaderIndex index) const;
    [[nodiscard]] const PipelineLayout& get_pipeline_layout(void) const;
    [[nodiscard]] const DescriptorSetLayout& get_descriptor_set_layout(DescriptorUpdateRate update_rate) const;
    [[nodiscard]] const std::vector<VertexBindingInfo>& get_vertex_bindings(void) const;

private:
    ShaderSetInfo   _info{};
    PipelineLayout* _pipeline_layout{ nullptr };
};

}

#endif
