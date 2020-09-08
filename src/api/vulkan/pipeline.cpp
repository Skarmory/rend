#include "pipeline.h"

#include "device_context.h"
#include "logical_device.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "shader.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

#include <cstring>

using namespace rend;

Pipeline::~Pipeline(void)
{
    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->destroy_pipeline(_vk_pipeline);
}

StatusCode Pipeline::create_pipeline(PipelineLayout& layout, RenderPass& render_pass, uint32_t subpass)
{
    if(_vk_pipeline != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_infos;
    for(Shader* shader : _shaders)
    {
        shader_stage_infos.push_back(_gen_shader_stage_create_info(*shader));
    }

    std::vector<VkPipelineColorBlendAttachmentState> vk_attachments;
    for(ColourBlendAttachmentSettings& settings : _colour_blend_attachment_settings)
    {
        vk_attachments.push_back(_gen_colour_blend_attachment_state(settings));
    }

    std::vector<VkDynamicState> dynamic_states;
    for(DynamicState state : _dynamic_states)
    {
        dynamic_states.push_back(vulkan_helpers::convert_dynamic_state(state));
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info     = _gen_vertex_input_state_create_info();
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = _gen_input_assembly_state_create_info();
    VkPipelineTessellationStateCreateInfo tessellation_info    = _gen_tessellation_state_create_info();
    VkPipelineViewportStateCreateInfo viewport_info            = _gen_viewport_state_create_info();
    VkPipelineRasterizationStateCreateInfo rasterisation_info  = _gen_rasterisation_state_create_info();
    VkPipelineMultisampleStateCreateInfo multisample_info      = _gen_multisample_state_create_info();
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info   = _gen_depth_stencil_state_create_info();
    VkPipelineColorBlendStateCreateInfo colour_blend_info      = _gen_colour_blend_state_create_info(vk_attachments);
    VkPipelineDynamicStateCreateInfo dynamic_info              = _gen_dynamic_state_create_info(dynamic_states);

    VkGraphicsPipelineCreateInfo pipeline_create_info = vulkan_helpers::gen_graphics_pipeline_create_info();
    pipeline_create_info.stageCount          = static_cast<uint32_t>(shader_stage_infos.size());
    pipeline_create_info.pStages             = shader_stage_infos.data();
    pipeline_create_info.pVertexInputState   = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly_info;
    pipeline_create_info.pTessellationState  = &tessellation_info;
    pipeline_create_info.pViewportState      = &viewport_info;
    pipeline_create_info.pRasterizationState = &rasterisation_info;
    pipeline_create_info.pMultisampleState   = &multisample_info;
    pipeline_create_info.pDepthStencilState  = &depth_stencil_info;
    pipeline_create_info.pColorBlendState    = &colour_blend_info;
    pipeline_create_info.pDynamicState       = &dynamic_info;
    pipeline_create_info.layout              = layout.get_handle();
    pipeline_create_info.renderPass          = render_pass.get_handle();
    pipeline_create_info.subpass             = subpass;
    pipeline_create_info.basePipelineHandle  = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex   = 0;

    _vk_pipeline = static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->create_pipeline(pipeline_create_info);

    if(_vk_pipeline == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    _layout      = &layout;
    _render_pass = &render_pass;
    _subpass     = subpass;

    return StatusCode::SUCCESS;
}

void Pipeline::add_shader(Shader& shader)
{
    _shaders.push_back(&shader);
}

void Pipeline::add_vertex_binding_description(uint32_t binding, uint32_t stride)
{
    _vk_binding_descs.push_back({
        binding, stride, VK_VERTEX_INPUT_RATE_VERTEX
    });
}

void Pipeline::add_vertex_attribute_description(uint32_t location, uint32_t binding, Format format, uint32_t offset)
{
    _vk_attribute_descs.push_back({
        location, binding, vulkan_helpers::convert_format(format), offset
    });
}

void Pipeline::add_viewport(float x, float y, float width, float height, float min_depth, float max_depth)
{
    _vk_viewports.push_back({ x, y, width, height, min_depth, max_depth });
}

void Pipeline::add_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    _vk_scissors.push_back({ x, y, width, height });
}

void Pipeline::set_topology(Topology topology)
{
    _topology = topology;
}

void Pipeline::set_primitive_restart(bool enable)
{
    _primitive_restart = enable;
}

void Pipeline::set_patch_control_points(uint32_t patch_control_points)
{
    _patch_control_points = patch_control_points;
}

void Pipeline::set_depth_clamp(bool enable)
{
    _depth_clamp_enable = enable;
}

void Pipeline::set_rasteriser_discard(bool enable)
{
    _rasteriser_discard_enable = enable;
}

void Pipeline::set_polygon_mode(PolygonMode mode)
{
    _polygon_mode = mode;
}

void Pipeline::set_cull_mode(CullMode mode)
{
    _cull_mode = mode;
}

void Pipeline::set_front_face(FrontFace face)
{
    _front_face = face;
}

void Pipeline::set_depth_bias(bool enable, float constant_factor, float clamp, float slope_factor)
{
    _depth_bias_enable = enable;
    _depth_bias_constant = constant_factor;
    _depth_bias_clamp = clamp;
    _depth_bias_slope = slope_factor;
}

void Pipeline::set_line_width(float width)
{
    _line_width = width;
}

void Pipeline::set_samples(MSAASamples samples)
{
    _samples = samples;
}

void Pipeline::set_sample_shading(bool enable)
{
    _sample_shading_enable = enable;
}

void Pipeline::set_min_sample_shading(float min_sample_shading)
{
    _min_sample_shading = min_sample_shading;
}

void Pipeline::set_sample_mask(uint32_t sample_mask)
{
    _sample_mask = sample_mask;
}

void Pipeline::set_alpha_to_coverage(bool enable)
{
    _alpha_to_coverage_enable = enable;
}

void Pipeline::set_alpha_to_one(bool enable)
{
    _alpha_to_one_enable = enable;
}

void Pipeline::set_depth_test(bool enable)
{
    _depth_test_enable = enable;
}

void Pipeline::set_depth_write(bool enable)
{
    _depth_write_enable = enable;
}

void Pipeline::set_compare_op(CompareOp compare_op)
{
    _compare_op = compare_op;
}

void Pipeline::set_depth_bounds_test(bool enable)
{
    _depth_bounds_test_enable = enable;
}

void Pipeline::set_stencil_test(bool enable)
{
    _stencil_test_enable = enable;
}

void Pipeline::set_front_stencil(StencilOp fail_op, StencilOp success_op, StencilOp depth_fail_op, CompareOp comp_op, uint32_t comp_mask, uint32_t write_mask, uint32_t reference)
{
    _stencil_op_state_front.fail_op = fail_op;
    _stencil_op_state_front.success_op = success_op;
    _stencil_op_state_front.depth_fail_op = depth_fail_op;
    _stencil_op_state_front.comp_op = comp_op;
    _stencil_op_state_front.comp_mask = comp_mask;
    _stencil_op_state_front.write_mask = write_mask;
    _stencil_op_state_front.reference = reference;
}

void Pipeline::set_back_stencil(StencilOp fail_op, StencilOp success_op, StencilOp depth_fail_op, CompareOp comp_op, uint32_t comp_mask, uint32_t write_mask, uint32_t reference)
{
    _stencil_op_state_back.fail_op = fail_op;
    _stencil_op_state_back.success_op = success_op;
    _stencil_op_state_back.depth_fail_op = depth_fail_op;
    _stencil_op_state_back.comp_op = comp_op;
    _stencil_op_state_back.comp_mask = comp_mask;
    _stencil_op_state_back.write_mask = write_mask;
    _stencil_op_state_back.reference = reference;
}

void Pipeline::set_depth_bounds(float min, float max)
{
    _min_depth_bound = min;
    _max_depth_bound = max;
}

void Pipeline::set_logic_op(bool enable, LogicOp op)
{
    _logic_op_enable = enable;
    _logic_op = op;
}

void Pipeline::set_blend_constants(float c1, float c2, float c3, float c4)
{
    _blend_constants[0] = c1;
    _blend_constants[1] = c2;
    _blend_constants[2] = c3;
    _blend_constants[3] = c4;
}

void Pipeline::add_colour_blend_attachment_state(
    bool blend_enable, uint32_t colour_write_mask,
    BlendFactor colour_blend_src_factor, BlendFactor colour_blend_dst_factor, BlendOp colour_blend_op,
    BlendFactor alpha_blend_src_factor, BlendFactor alpha_blend_dst_factor, BlendOp alpha_blend_op
)
{
    _colour_blend_attachment_settings.push_back({
        blend_enable, colour_blend_src_factor, colour_blend_dst_factor,
        colour_blend_op, alpha_blend_src_factor, alpha_blend_dst_factor, alpha_blend_op,
        colour_write_mask
    });
}

void Pipeline::add_dynamic_state(DynamicState state)
{
    _dynamic_states.insert(state);
}

VkPipeline Pipeline::get_handle(void) const
{
    return _vk_pipeline;
}

VkPipelineShaderStageCreateInfo Pipeline::_gen_shader_stage_create_info(Shader& shader)
{
    VkPipelineShaderStageCreateInfo create_info = vulkan_helpers::gen_shader_stage_create_info();
    create_info.module = shader.get_handle();
    create_info.pName = "main";
    create_info.pSpecializationInfo = nullptr;

    switch(shader.shader_type())
    {
        case ShaderType::VERTEX:
            create_info.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
        case ShaderType::FRAGMENT:
            create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
    }

    return create_info;
}

VkPipelineVertexInputStateCreateInfo Pipeline::_gen_vertex_input_state_create_info(void)
{
    VkPipelineVertexInputStateCreateInfo create_info = vulkan_helpers::gen_vertex_input_state_create_info();
    create_info.vertexBindingDescriptionCount        = static_cast<uint32_t>(_vk_binding_descs.size());
    create_info.pVertexBindingDescriptions           = _vk_binding_descs.data();
    create_info.vertexAttributeDescriptionCount      = static_cast<uint32_t>(_vk_attribute_descs.size());
    create_info.pVertexAttributeDescriptions         = _vk_attribute_descs.data();

    return create_info;
}

VkPipelineInputAssemblyStateCreateInfo Pipeline::_gen_input_assembly_state_create_info(void)
{
    VkPipelineInputAssemblyStateCreateInfo create_info = vulkan_helpers::gen_input_assembly_state_create_info();
    create_info.topology               = vulkan_helpers::convert_topology(_topology);
    create_info.primitiveRestartEnable = _primitive_restart;

    return create_info;
}

VkPipelineTessellationStateCreateInfo  Pipeline::_gen_tessellation_state_create_info(void)
{
    VkPipelineTessellationStateCreateInfo create_info = vulkan_helpers::gen_tessellation_state_create_info();
    create_info.patchControlPoints = _patch_control_points;

    return create_info;
}

VkPipelineViewportStateCreateInfo Pipeline::_gen_viewport_state_create_info(void)
{
    VkPipelineViewportStateCreateInfo create_info = vulkan_helpers::gen_viewport_state_create_info();
    create_info.viewportCount = static_cast<uint32_t>(_vk_viewports.size());
    create_info.pViewports    = _vk_viewports.data();
    create_info.scissorCount  = static_cast<uint32_t>(_vk_scissors.size());
    create_info.pScissors     = _vk_scissors.data();

    return create_info;
}

VkPipelineRasterizationStateCreateInfo Pipeline::_gen_rasterisation_state_create_info(void)
{
    VkPipelineRasterizationStateCreateInfo create_info = vulkan_helpers::gen_rasterisation_state_create_info();
    create_info.depthClampEnable        = static_cast<VkBool32>(_depth_clamp_enable);
    create_info.rasterizerDiscardEnable = static_cast<VkBool32>(_rasteriser_discard_enable);
    create_info.polygonMode             = vulkan_helpers::convert_polygon_mode(_polygon_mode);
    create_info.cullMode                = vulkan_helpers::convert_cull_mode(_cull_mode);
    create_info.frontFace               = vulkan_helpers::convert_front_face(_front_face);
    create_info.depthBiasEnable         = static_cast<VkBool32>(_depth_bias_enable);
    create_info.depthBiasConstantFactor = _depth_bias_constant;
    create_info.depthBiasClamp          = _depth_bias_clamp;
    create_info.depthBiasSlopeFactor    = _depth_bias_slope;
    create_info.lineWidth               = _line_width;

    return create_info;
}

VkPipelineMultisampleStateCreateInfo Pipeline::_gen_multisample_state_create_info(void)
{
    VkPipelineMultisampleStateCreateInfo create_info = vulkan_helpers::gen_multisample_state_create_info();
    create_info.rasterizationSamples  = vulkan_helpers::convert_sample_count(_samples);
    create_info.sampleShadingEnable   = _sample_shading_enable;
    create_info.minSampleShading      = _min_sample_shading;
    create_info.pSampleMask           = &_sample_mask;
    create_info.alphaToCoverageEnable = _alpha_to_coverage_enable;
    create_info.alphaToOneEnable      = _alpha_to_one_enable;

    return create_info;
}

VkPipelineDepthStencilStateCreateInfo Pipeline::_gen_depth_stencil_state_create_info(void)
{
    VkPipelineDepthStencilStateCreateInfo create_info = vulkan_helpers::gen_depth_stencil_state_create_info();
    create_info.depthTestEnable       = static_cast<VkBool32>(_depth_test_enable);
    create_info.depthWriteEnable      = static_cast<VkBool32>(_depth_write_enable);
    create_info.depthCompareOp        = vulkan_helpers::convert_compare_op(_compare_op);
    create_info.depthBoundsTestEnable = static_cast<VkBool32>(_depth_bounds_test_enable);
    create_info.stencilTestEnable     = static_cast<VkBool32>(_stencil_test_enable);
    create_info.front.failOp          = vulkan_helpers::convert_stencil_op(_stencil_op_state_front.fail_op);
    create_info.front.passOp          = vulkan_helpers::convert_stencil_op(_stencil_op_state_front.success_op);
    create_info.front.depthFailOp     = vulkan_helpers::convert_stencil_op(_stencil_op_state_front.depth_fail_op);
    create_info.front.compareOp       = vulkan_helpers::convert_compare_op(_stencil_op_state_front.comp_op);
    create_info.front.compareMask     = _stencil_op_state_front.comp_mask;
    create_info.front.writeMask       = _stencil_op_state_front.write_mask;
    create_info.front.reference       = _stencil_op_state_front.reference;
    create_info.back.failOp           = vulkan_helpers::convert_stencil_op(_stencil_op_state_back.fail_op);
    create_info.back.passOp           = vulkan_helpers::convert_stencil_op(_stencil_op_state_back.success_op);
    create_info.back.depthFailOp      = vulkan_helpers::convert_stencil_op(_stencil_op_state_back.depth_fail_op);
    create_info.back.compareOp        = vulkan_helpers::convert_compare_op(_stencil_op_state_back.comp_op);
    create_info.back.compareMask      = _stencil_op_state_back.comp_mask;
    create_info.back.writeMask        = _stencil_op_state_back.write_mask;
    create_info.back.reference        = _stencil_op_state_back.reference;
    create_info.minDepthBounds        = _min_depth_bound;
    create_info.maxDepthBounds        = _max_depth_bound;

    return create_info;
}

VkPipelineColorBlendAttachmentState Pipeline::_gen_colour_blend_attachment_state(ColourBlendAttachmentSettings& settings)
{
    VkPipelineColorBlendAttachmentState state = {};
    state.blendEnable = settings.blend_enable;
    state.srcColorBlendFactor = vulkan_helpers::convert_blend_factor(settings.colour_blend_src_factor);
    state.dstColorBlendFactor = vulkan_helpers::convert_blend_factor(settings.colour_blend_dst_factor);
    state.colorBlendOp = vulkan_helpers::convert_blend_op(settings.colour_blend_op);
    state.srcAlphaBlendFactor = vulkan_helpers::convert_blend_factor(settings.alpha_blend_src_factor);
    state.dstAlphaBlendFactor = vulkan_helpers::convert_blend_factor(settings.alpha_blend_dst_factor);
    state.alphaBlendOp = vulkan_helpers::convert_blend_op(settings.alpha_blend_op);
    state.colorWriteMask = static_cast<VkColorComponentFlags>(settings.write_mask);

    return state;
}

VkPipelineColorBlendStateCreateInfo Pipeline::_gen_colour_blend_state_create_info(std::vector<VkPipelineColorBlendAttachmentState>& states)
{
    VkPipelineColorBlendStateCreateInfo create_info = vulkan_helpers::gen_colour_blend_state_create_info();
    create_info.logicOpEnable     = _logic_op_enable;
    create_info.logicOp           = vulkan_helpers::convert_logic_op(_logic_op);
    create_info.attachmentCount   = static_cast<uint32_t>(states.size());
    create_info.pAttachments      = states.data();
    create_info.blendConstants[0] = _blend_constants[0];
    create_info.blendConstants[1] = _blend_constants[1];
    create_info.blendConstants[2] = _blend_constants[2];
    create_info.blendConstants[3] = _blend_constants[3];

    return create_info;
}

VkPipelineDynamicStateCreateInfo Pipeline::_gen_dynamic_state_create_info(std::vector<VkDynamicState>& states)
{
    VkPipelineDynamicStateCreateInfo create_info = vulkan_helpers::gen_dynamic_state_create_info();
    create_info.dynamicStateCount = static_cast<uint32_t>(states.size());
    create_info.pDynamicStates    = states.data();

    return create_info;
}
