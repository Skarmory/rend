#include "pipeline.h"

#include "logical_device.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "shader.h"
#include "utils.h"

#include <cstring>

using namespace rend;

Pipeline::Pipeline(LogicalDevice* device, PipelineSettings* settings) : _device(device)
{
    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_infos(settings->shader_stage_shaders.size());
    for(size_t idx = 0; idx < shader_stage_infos.size(); idx++)
    {
        VkShaderStageFlagBits stage;
        switch(settings->shader_stage_shaders[idx]->get_shader_type())
        {
            case ShaderType::VERTEX:
                stage = VK_SHADER_STAGE_VERTEX_BIT; break;

            case ShaderType::FRAGMENT:
                stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
        }

        shader_stage_infos[idx] =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = stage,
            .module = settings->shader_stage_shaders[idx]->get_handle(),
            .pName = "main",
            .pSpecializationInfo = nullptr
        };
    }

    VkPipelineVertexInputStateCreateInfo input_info =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount   = static_cast<uint32_t>(settings->vertex_input_binding_descs.size()),
        .pVertexBindingDescriptions      = settings->vertex_input_binding_descs.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(settings->vertex_input_attribute_descs.size()),
        .pVertexAttributeDescriptions    = settings->vertex_input_attribute_descs.data()
    };

    VkPipelineInputAssemblyStateCreateInfo assembly_info =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = settings->input_assembly_topology,
        .primitiveRestartEnable = static_cast<VkBool32>(settings->input_assembly_restart_enable)
    };

    VkPipelineTessellationStateCreateInfo tessellation_info =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .patchControlPoints = settings->tessellation_patch_control_points
    };

    VkPipelineViewportStateCreateInfo viewport_info =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = static_cast<uint32_t>(settings->viewport_viewports.size()),
        .pViewports    = settings->viewport_viewports.data(),
        .scissorCount  = static_cast<uint32_t>(settings->viewport_scissors.size()),
        .pScissors     = settings->viewport_scissors.data()
    };

    VkPipelineRasterizationStateCreateInfo rasterisation_info =
    {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = static_cast<VkBool32>(settings->rasterisation_depth_clamp_enable),
        .rasterizerDiscardEnable = static_cast<VkBool32>(settings->rasterisation_discard_enable),
        .polygonMode             = settings->rasterisation_polygon_mode,
        .cullMode                = settings->rasterisation_cull_mode,
        .frontFace               = settings->rasterisation_front_face,
        .depthBiasEnable         = static_cast<VkBool32>(settings->rasterisation_depth_bias_enable),
        .depthBiasConstantFactor = settings->rasterisation_depth_bias_constant_factor,
        .depthBiasClamp          = settings->rasterisation_depth_bias_clamp,
        .depthBiasSlopeFactor    = settings->rasterisation_depth_bias_slope_factor,
        .lineWidth               = settings->rasterisation_line_width
    };

    VkPipelineMultisampleStateCreateInfo multisample_info =
    {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .rasterizationSamples  = settings->multisample_samples,
        .sampleShadingEnable   = settings->multisample_sample_shading_enable,
        .minSampleShading      = settings->multisample_min_sample_shading,
        .pSampleMask           = settings->multisample_sample_mask,
        .alphaToCoverageEnable = settings->multisample_alpha_to_coverage_enable,
        .alphaToOneEnable      = settings->multisample_alpha_to_one_enable
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info =
    {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .depthTestEnable       = static_cast<VkBool32>(settings->depth_stencil_depth_test_enable),
        .depthWriteEnable      = static_cast<VkBool32>(settings->depth_stencil_depth_write_enable),
        .depthCompareOp        = settings->depth_stencil_depth_compare_op,
        .depthBoundsTestEnable = static_cast<VkBool32>(settings->depth_stencil_depth_bounds_test_enable),
        .stencilTestEnable     = static_cast<VkBool32>(settings->depth_stencil_stencil_test_enable),
        .front                 = settings->depth_stencil_front,
        .back                  = settings->depth_stencil_back,
        .minDepthBounds        = settings->depth_stencil_min_depth_bounds,
        .maxDepthBounds        = settings->depth_stencil_max_depth_bounds
    };

    VkPipelineColorBlendStateCreateInfo colour_blend_info =
    {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0,
        .logicOpEnable     = static_cast<VkBool32>(settings->colour_blend_logic_op_enable),
        .logicOp           = settings->colour_blend_logic_op,
        .attachmentCount   = static_cast<uint32_t>(settings->colour_blend_attachments.size()),
        .pAttachments      = settings->colour_blend_attachments.data(),
        .blendConstants    = { settings->colour_blend_blend_constants[0], settings->colour_blend_blend_constants[1], settings->colour_blend_blend_constants[2], settings->colour_blend_blend_constants[3] }
    };

    VkPipelineDynamicStateCreateInfo dynamic_info =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(settings->dynamic_state_states.size()),
        .pDynamicStates    = settings->dynamic_state_states.data()
    };

    VkGraphicsPipelineCreateInfo create_info =
    {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stageCount          = static_cast<uint32_t>(shader_stage_infos.size()),
        .pStages             = shader_stage_infos.data(),
        .pVertexInputState   = &input_info,
        .pInputAssemblyState = &assembly_info,
        .pTessellationState  = &tessellation_info,
        .pViewportState      = &viewport_info,
        .pRasterizationState = &rasterisation_info,
        .pMultisampleState   = &multisample_info,
        .pDepthStencilState  = &depth_stencil_info,
        .pColorBlendState    = &colour_blend_info,
        .pDynamicState       = &dynamic_info,
        .layout              = settings->layout->get_handle(),
        .renderPass          = settings->render_pass->get_handle(),
        .subpass             = settings->subpass,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = 0
    };

    VULKAN_DEATH_CHECK(vkCreateGraphicsPipelines(_device->get_handle(), VK_NULL_HANDLE, 1, &create_info, nullptr, &_vk_pipeline), "Failed to create graphics pipeline");
}

Pipeline::~Pipeline(void)
{
    vkDestroyPipeline(_device->get_handle(), _vk_pipeline, nullptr);
}

VkPipeline Pipeline::get_handle(void) const
{
    return _vk_pipeline;
}
