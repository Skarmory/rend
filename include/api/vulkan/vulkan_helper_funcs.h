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
VkFormat                convert_texture_format(TextureFormat format);
VkShaderStageFlags      convert_shader_stage(ShaderType type);
VkImageLayout           convert_image_layout(ImageLayout layout);
VkAttachmentLoadOp      convert_load_op(LoadOp op);
VkAttachmentStoreOp     convert_store_op(StoreOp op);
VkAccessFlagBits        convert_memory_access(MemoryAccess access);
VkAccessFlags           convert_memory_accesses(MemoryAccesses accesses);
VkPipelineStageFlagBits convert_pipeline_stage(PipelineStage stage);
VkPipelineStageFlags    convert_pipeline_stages(PipelineStages stages);

}

}

#endif
#endif
