#include "core/shader_set.h"

#include "core/renderer.h"
#include "core/rend_service.h"

using namespace rend;

ShaderSet::ShaderSet(const std::string& name, const ShaderSetInfo& info)
    :
        _info(info),
        GPUResource(name)
{
    auto* rr = RendService::renderer();

    PipelineLayoutInfo pl_info{};
    pl_info.descriptor_set_layouts = _info.layouts.data();//_desc_set_layout_handles.data();
    pl_info.descriptor_set_layout_count = _info.layouts.size();//_desc_set_layout_handles.size();
    pl_info.push_constant_ranges = _info.push_constant_ranges.data();
    pl_info.push_constant_range_count = _info.push_constant_ranges.size();
    _pipeline_layout_handle = rr->create_pipeline_layout(name + " pipeline layout", pl_info);
}

ShaderSet::~ShaderSet(void)
{
}

ShaderHandle ShaderSet::get_shader(ShaderIndex index) const
{
    return _info.shaders[index];
}

const std::vector<VertexAttributeInfo>& ShaderSet::get_vertex_attribute_infos(void) const
{
    return _info.vertex_attribute_infos;
}

PipelineLayoutHandle ShaderSet::get_pipeline_layout(void) const
{
    return _pipeline_layout_handle;
}

DescriptorSetLayoutHandle ShaderSet::get_descriptor_set_layout(DescriptorFrequency freq) const
{
    return _info.layouts[freq];
}
