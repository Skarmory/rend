#ifndef REND_CORE_SHADER_SET_H
#define REND_CORE_SHADER_SET_H

#include "core/descriptor_set_binding.h"
#include "core/descriptor_set_layout_binding.h"
#include "core/rend_defs.h"
#include "core/gpu_resource.h"

#include <array>
#include <string>
#include <vector>

namespace rend
{

enum DescriptorFrequency
{
    VIEW = 0,
    MATERIAL
};
constexpr size_t DESCRIPTOR_FREQUENCY_COUNT = 2;

struct ShaderSetInfo
{
    std::array<ShaderHandle, SHADER_STAGE_COUNT>         shaders;
    std::vector<VertexAttributeInfo>                     vertex_attribute_infos;
    //std::array<std::vector<DescriptorSetLayoutBinding>, DESCRIPTOR_FREQUENCY_COUNT> layout_bindings;
    std::array<DescriptorSetLayoutHandle, DESCRIPTOR_FREQUENCY_COUNT> layouts;
    std::vector<PushConstantRange>                       push_constant_ranges;

};

class ShaderSet : public GPUResource
{
    public:
        ShaderSet(const std::string& name, const ShaderSetInfo& info);
        ~ShaderSet(void);

        [[nodiscard]] ShaderHandle get_shader(ShaderIndex index) const;
        [[nodiscard]] const std::vector<VertexAttributeInfo>& get_vertex_attribute_infos(void) const;
        [[nodiscard]] PipelineLayoutHandle get_pipeline_layout(void) const;
        [[nodiscard]] DescriptorSetLayoutHandle get_descriptor_set_layout(DescriptorFrequency freq) const;

    private:
        ShaderSetInfo                                _info{};
        PipelineLayoutHandle                         _pipeline_layout_handle{ NULL_HANDLE };
};

}

#endif
