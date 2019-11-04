#ifdef USE_VULKAN
#ifndef REND_VULKAN_HELPER_FUNCS_H
#define REND_VULKAN_HELPER_FUNCS_H

#include <vulkan.h>

#include "rend_defs.h"

namespace rend
{

namespace vulkan_helpers
{

VkImageType             find_image_type(VkExtent3D extent);
VkImageViewType         find_image_view_type(VkImageType image_type, bool array, bool cube);
VkImageAspectFlags      find_image_aspects(VkFormat format);
VkFormat                convert_format(Format format);
VkShaderStageFlags      convert_shader_stage(ShaderType type);
VkImageLayout           convert_image_layout(ImageLayout layout);
VkAttachmentLoadOp      convert_load_op(LoadOp op);
VkAttachmentStoreOp     convert_store_op(StoreOp op);
VkAccessFlagBits        convert_memory_access(MemoryAccess access);
VkAccessFlags           convert_memory_accesses(MemoryAccesses accesses);
VkPipelineStageFlagBits convert_pipeline_stage(PipelineStage stage);
VkPipelineStageFlags    convert_pipeline_stages(PipelineStages stages);
VkPrimitiveTopology     convert_topology(Topology topology);
VkPolygonMode           convert_polygon_mode(PolygonMode mode);
VkFrontFace             convert_front_face(FrontFace face);
VkCullModeFlagBits      convert_cull_mode(CullMode mode);
VkSampleCountFlagBits   convert_sample_count(uint32_t count);
VkCompareOp             convert_compare_op(CompareOp compare_op);
VkStencilOp             convert_stencil_op(StencilOp stencil_op);
VkLogicOp               convert_logic_op(LogicOp logic_op);
VkBlendFactor           convert_blend_factor(BlendFactor factor);
VkBlendOp               convert_blend_op(BlendOp op);
VkDynamicState          convert_dynamic_state(DynamicState state);

VkMemoryAllocateInfo     gen_memory_allocate_info(void);
VkSamplerCreateInfo      gen_sampler_create_info(void);
VkImageCreateInfo        gen_image_create_info(void);
VkImageViewCreateInfo    gen_image_view_create_info(void);
VkBufferCreateInfo       gen_buffer_create_info(void);
VkShaderModuleCreateInfo gen_shader_module_create_info(void);

}

}

#endif
#endif
