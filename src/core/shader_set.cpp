#include "core/shader_set.h"

using namespace rend;

ShaderSet::ShaderSet(const std::string& name, const ShaderSetInfo& info, PipelineLayout* layout)
    :
        GPUResource(name),
        _info(info),
        _pipeline_layout(layout)
{
}

ShaderSet::~ShaderSet(void)
{
}

const Shader* ShaderSet::get_shader(ShaderIndex index) const
{
    return _info.shaders[index];
}

const PipelineLayout& ShaderSet::get_pipeline_layout(void) const
{
    return *_pipeline_layout;
}

const DescriptorSetLayout& ShaderSet::get_descriptor_set_layout(DescriptorFrequency freq) const
{
    return *_info.layouts[freq];
}

const std::vector<VertexBindingInfo>& ShaderSet::get_vertex_bindings(void) const
{
    return _info.binding_info;
}
