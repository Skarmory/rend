#ifndef REND_PIPELINE_H
#define REND_PIPELINE_H

// TODO: Split Pipeline creation into bespoke PipelineBuilder class?

#include "rend_defs.h"

#include <unordered_set>
#include <vulkan.h>
#include <vector>

namespace rend
{

class DeviceContext;
class PipelineLayout;
class RenderPass;
class Shader;

typedef VkRect2D VkScissorArea;

class Pipeline
{
public:
    explicit Pipeline(DeviceContext& device);
    ~Pipeline(void);

    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&)      = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&)      = delete;

    StatusCode create_pipeline(PipelineLayout& layout, RenderPass& render_pass, uint32_t subpass);

    void add_shader(Shader& shader);

    // Vertex Input State
    void add_vertex_binding_description(uint32_t binding, uint32_t stride);
    void add_vertex_attribute_description(uint32_t location, uint32_t binding, Format format, uint32_t offset);

    // Input Assembly State
    void set_topology(Topology topology);
    void set_primitive_restart(bool enable);

    // Tessellation State
    void set_patch_control_points(uint32_t patch_control_points);

    // Viewport State
    void add_viewport(float x, float y, float width, float height, float min_depth, float max_depth);
    void add_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height);

    // Rasterisation State
    void set_depth_clamp(bool enable);
    void set_rasteriser_discard(bool enable);
    void set_polygon_mode(PolygonMode mode);
    void set_cull_mode(CullMode mode);
    void set_front_face(FrontFace face);
    void set_depth_bias(bool enable, float constant_factor, float clamp, float slope_factor);
    void set_line_width(float width);

    // Multisample State
    void set_samples(uint32_t samples);
    void set_sample_shading(bool enable);
    void set_min_sample_shading(float min_sample_shading);
    void set_sample_mask(uint32_t sample_mask);
    void set_alpha_to_coverage(bool enable);
    void set_alpha_to_one(bool enable);

    // Depth Stencil State
    void set_depth_test(bool enable);
    void set_depth_write(bool enable);
    void set_compare_op(CompareOp compare_op);
    void set_depth_bounds_test(bool enable);
    void set_stencil_test(bool enable);
    void set_front_stencil(StencilOp fail_op, StencilOp success_op, StencilOp depth_fail_op, CompareOp comp_op, uint32_t comp_mask, uint32_t write_mask, uint32_t reference);
    void set_back_stencil(StencilOp fail_op, StencilOp success_op, StencilOp depth_fail_op, CompareOp comp_op, uint32_t comp_mask, uint32_t write_mask, uint32_t reference);
    void set_depth_bounds(uint32_t min, uint32_t max);

    // Colour Blend State
    void set_logic_op(bool enable, LogicOp op);
    void set_blend_constants(float c1, float c2, float c3, float c4);
    void add_colour_blend_attachment_state(
        bool blend_enable, uint32_t colour_write_mask,
        BlendFactor colour_blend_src_factor, BlendFactor colour_blend_dst_factor, BlendOp colour_blend_op,
        BlendFactor alpha_blend_src_factor, BlendFactor alpha_blend_dst_factor, BlendOp alpha_blend_op
    );

    // Dynamic State
    void add_dynamic_state(DynamicState state);

    VkPipeline get_handle(void) const;

private:
    struct ColourBlendAttachmentSettings;

    VkPipelineShaderStageCreateInfo        _gen_shader_stage_create_info(Shader& shader);
    VkPipelineVertexInputStateCreateInfo   _gen_vertex_input_state_create_info(void);
    VkPipelineInputAssemblyStateCreateInfo _gen_input_assembly_state_create_info(void);
    VkPipelineTessellationStateCreateInfo  _gen_tessellation_state_create_info(void);
    VkPipelineViewportStateCreateInfo      _gen_viewport_state_create_info(void);
    VkPipelineRasterizationStateCreateInfo _gen_rasterisation_state_create_info(void);
    VkPipelineMultisampleStateCreateInfo   _gen_multisample_state_create_info(void);
    VkPipelineDepthStencilStateCreateInfo  _gen_depth_stencil_state_create_info(void);
    VkPipelineColorBlendAttachmentState    _gen_colour_blend_attachment_state(ColourBlendAttachmentSettings& settings);
    VkPipelineColorBlendStateCreateInfo    _gen_colour_blend_state_create_info(std::vector<VkPipelineColorBlendAttachmentState>& states);
    VkPipelineDynamicStateCreateInfo       _gen_dynamic_state_create_info(std::vector<VkDynamicState>& states);

private:
    struct StencilOpState
    {
        StencilOp fail_op;
        StencilOp success_op;
        StencilOp depth_fail_op;
        CompareOp comp_op;
        uint32_t  comp_mask;
        uint32_t  write_mask;
        uint32_t  reference;
    };

    struct ColourBlendAttachmentSettings
    {
        bool        blend_enable;
        BlendFactor colour_blend_src_factor;
        BlendFactor colour_blend_dst_factor;
        BlendOp     colour_blend_op;
        BlendFactor alpha_blend_src_factor;
        BlendFactor alpha_blend_dst_factor;
        BlendOp     alpha_blend_op;
        uint32_t    write_mask;
    };

    DeviceContext&   _context;

    PipelineLayout* _layout;
    RenderPass*     _render_pass;
    int32_t         _subpass;

    std::vector<Shader*> _shaders;

    // Vertex input state settings
    std::vector<VkVertexInputBindingDescription>   _vk_binding_descs;
    std::vector<VkVertexInputAttributeDescription> _vk_attribute_descs;

    // Input assembly state settings
    Topology _topology;
    bool     _primitive_restart;

    // Tessellation state settings
    uint32_t _patch_control_points;

    // Viewport state settings
    std::vector<VkViewport>    _vk_viewports;
    std::vector<VkScissorArea> _vk_scissors;

    // Rasteriser state settings
    bool        _depth_clamp_enable;
    bool        _rasteriser_discard_enable;
    PolygonMode _polygon_mode;
    CullMode    _cull_mode;
    FrontFace   _front_face;
    bool        _depth_bias_enable;
    float       _depth_bias_constant;
    float       _depth_bias_clamp;
    float       _depth_bias_slope;
    float       _line_width;

    // Multisample state settings
    uint32_t _samples;
    bool     _sample_shading_enable;
    float    _min_sample_shading;
    uint32_t _sample_mask;
    bool     _alpha_to_coverage_enable;
    bool     _alpha_to_one_enable;

    // Depth stencil settings
    bool           _depth_test_enable;
    bool           _depth_write_enable;
    CompareOp      _compare_op;
    bool           _depth_bounds_test_enable;
    bool           _stencil_test_enable;
    StencilOpState _stencil_op_state_front;
    StencilOpState _stencil_op_state_back;
    uint32_t       _min_depth_bound;
    uint32_t       _max_depth_bound;

    // Colour blend settings
    bool    _logic_op_enable;
    LogicOp _logic_op;
    float   _blend_constants[4];
    std::vector<ColourBlendAttachmentSettings> _colour_blend_attachment_settings;

    // Dynamic state settings
    std::unordered_set<DynamicState> _dynamic_states;

    VkPipeline _vk_pipeline;
};

}

#endif
