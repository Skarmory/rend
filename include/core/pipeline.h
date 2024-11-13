#ifndef REND_CORE_PIPELINE_H
#define REND_CORE_PIPELINE_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

class PipelineLayout;
class RenderPass;
class Shader;

struct PipelineInfo
{
    // Vertex Input State
    //std::vector<VertexBindingInfo> vertex_binding_info{};
    //VertexAttributeInfo vertex_attribute_infos[constants::max_vertex_attributes];
    //uint32_t            vertex_attribute_info_count{ 0 };

    // Input Assembly State
    Topology topology{ Topology::TRIANGLE_LIST };
    bool     primitive_restart{ false };

    // Tessellation State
    uint32_t patch_control_points{ 0 };

    // Viewport State
    ViewportInfo viewport_info[constants::max_viewports];
    uint32_t     viewport_info_count{ 0 };
    ViewportInfo scissor_info[constants::max_scissors];
    uint32_t     scissor_info_count{ 0 };

    RasteriserInfo     rasteriser_info{};
    MultisamplingInfo  multisampling_info{};
    DepthStencilInfo   depth_stencil_info{};
    ColourBlendingInfo colour_blending_info{};
    DynamicStates      dynamic_states{ (uint32_t)DynamicState::NONE };

    //const PipelineLayout*  layout{ nullptr };
    ShaderSet* shader_set{ nullptr };
    RenderPass* render_pass{ nullptr };
    int32_t subpass{ 0 };
};

class Pipeline : public GPUResource, public RendObject
{
public:
    Pipeline(const std::string& name, const PipelineInfo& info);
    virtual ~Pipeline(void) = default;
    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;

    [[nodiscard]] const ShaderSet& get_shader_set(void) const;

    const PipelineInfo& pipeline_info(void) const;

private:
    PipelineInfo _info{};
};

}

#endif
