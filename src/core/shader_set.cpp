#include "core/shader_set.h"

using namespace rend;

ShaderSet::ShaderSet(const std::string& name, const ShaderSetInfo& info, RendHandle rend_handle, PipelineLayout* layout)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _info(info),
        _pipeline_layout(layout)
{
}

ShaderSet::~ShaderSet(void)
{
}

const Shader& ShaderSet::get_shader(ShaderIndex index) const
{
    return *_info.shaders[index];
}

const std::vector<VertexAttributeInfo>& ShaderSet::get_vertex_attribute_infos(void) const
{
    return _info.vertex_attribute_infos;
}

const PipelineLayout& ShaderSet::get_pipeline_layout(void) const
{
    return *_pipeline_layout;
}

const DescriptorSetLayout& ShaderSet::get_descriptor_set_layout(DescriptorFrequency freq) const
{
    return *_info.layouts[freq];
}
