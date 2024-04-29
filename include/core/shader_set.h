#ifndef REND_CORE_SHADER_SET_H
#define REND_CORE_SHADER_SET_H

#include "core/descriptor_set_binding.h"
#include "core/descriptor_set_layout_binding.h"
#include "core/rend_defs.h"
#include "core/gpu_resource.h"
#include "core/rend_object.h"

#include <array>
#include <string>
#include <vector>

namespace rend
{

class DescriptorSetLayout;
class PipelineLayout;
class Shader;

enum DescriptorFrequency
{
    VIEW = 0,
    MATERIAL = 1
};
constexpr size_t DESCRIPTOR_FREQUENCY_COUNT = 2;

struct ShaderSetInfo
{
    std::array<const Shader*, SHADER_STAGE_COUNT>                      shaders;
    std::vector<VertexAttributeInfo>                             vertex_attribute_infos;
    //std::array<std::vector<DescriptorSetLayoutBinding>, DESCRIPTOR_FREQUENCY_COUNT> layout_bindings;
    std::array<DescriptorSetLayout*, DESCRIPTOR_FREQUENCY_COUNT> layouts;
    std::vector<PushConstantRange>                               push_constant_ranges;

};

class ShaderSet : public GPUResource, public RendObject
{
public:
    ShaderSet(const std::string& name, const ShaderSetInfo& info, RendHandle rend_handle, PipelineLayout* layout);
    ~ShaderSet(void);

    [[nodiscard]] const Shader& get_shader(ShaderIndex index) const;
    [[nodiscard]] const std::vector<VertexAttributeInfo>& get_vertex_attribute_infos(void) const;
    [[nodiscard]] const PipelineLayout& get_pipeline_layout(void) const;
    [[nodiscard]] const DescriptorSetLayout& get_descriptor_set_layout(DescriptorFrequency freq) const;

private:
    ShaderSetInfo   _info{};
    PipelineLayout* _pipeline_layout{ nullptr };
};

}

#endif
