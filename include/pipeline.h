#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;
class PipelineLayout;
class RenderPass;
class Shader;

typedef VkRect2D VkScissorArea;

struct PipelineSettings
{
    // Shaders
    std::vector<Shader*> shader_stage_shaders;

    // Vertex input
    std::vector<VkVertexInputBindingDescription>   vertex_input_binding_descs;
    std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_descs;

    // Input assembly
    VkPrimitiveTopology input_assembly_topology       = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    bool                input_assembly_restart_enable = false;

    // Tessellation
    uint32_t tessellation_patch_control_points = 0;

    // Viewports
    std::vector<VkViewport>    viewport_viewports;
    std::vector<VkScissorArea> viewport_scissors;

    // Rasterisation
    bool            rasterisation_depth_clamp_enable         = false;
    bool            rasterisation_discard_enable             = false;
    VkPolygonMode   rasterisation_polygon_mode               = VK_POLYGON_MODE_FILL;
    VkCullModeFlags rasterisation_cull_mode                  = VK_CULL_MODE_BACK_BIT;
    VkFrontFace     rasterisation_front_face                 = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Make this a renderer global setting?
    bool            rasterisation_depth_bias_enable          = false;
    float           rasterisation_depth_bias_constant_factor = 1.0f;
    float           rasterisation_depth_bias_clamp           = 1.0f;
    float           rasterisation_depth_bias_slope_factor    = 1.0f;
    float           rasterisation_line_width                 = 1.0f;

    // Multisample
    VkSampleCountFlagBits multisample_samples                   = VK_SAMPLE_COUNT_1_BIT;
    bool                  multisample_sample_shading_enable     = false;
    float                 multisample_min_sample_shading        = 1.0f;
    const VkSampleMask*   multisample_sample_mask               = nullptr;
    bool                  multisample_alpha_to_coverage_enable  = false;
    bool                  multisample_alpha_to_one_enable       = false;

    // Depth stencil
    bool             depth_stencil_depth_test_enable            = true;
    bool             depth_stencil_stencil_test_enable          = false;
    bool             depth_stencil_depth_write_enable           = true;
    bool             depth_stencil_depth_bounds_test_enable     = false;
    VkCompareOp      depth_stencil_depth_compare_op             = VK_COMPARE_OP_LESS;
    VkStencilOpState depth_stencil_front                        = {};
    VkStencilOpState depth_stencil_back                         = {};
    float            depth_stencil_min_depth_bounds             = 0.0f;
    float            depth_stencil_max_depth_bounds             = 1.0f;

    // Colour blend
    bool                                             colour_blend_logic_op_enable    = false;
    VkLogicOp                                        colour_blend_logic_op           = VK_LOGIC_OP_CLEAR;
    std::vector<VkPipelineColorBlendAttachmentState> colour_blend_attachments;
    float                                            colour_blend_blend_constants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Dynamic state
    std::vector<VkDynamicState> dynamic_state_states;

    PipelineLayout* layout;
    RenderPass*     render_pass;
    uint32_t        subpass;
};

class Pipeline
{
    friend class LogicalDevice;

public:
    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&)      = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&)      = delete;

    VkPipeline get_handle(void) const;

private:
    Pipeline(LogicalDevice* device, PipelineSettings* settings);
    ~Pipeline(void);

private:
    VkPipeline _vk_pipeline;

    LogicalDevice* _device;
};

}

#endif
