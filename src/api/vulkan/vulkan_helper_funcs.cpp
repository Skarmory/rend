#include "vulkan_helper_funcs.h"
#include "gpu_texture_base.h"

#include <unordered_set>

using namespace rend;

std::unordered_set<VkFormat> depth_formats =
{
    VK_FORMAT_D16_UNORM,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_X8_D24_UNORM_PACK32,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D32_SFLOAT_S8_UINT
};

std::unordered_set<VkFormat> stencil_formats =
{
    VK_FORMAT_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT_S8_UINT
};

VkImageType vulkan_helpers::find_image_type(VkExtent3D extent)
{
    if(extent.depth > 1)
        return VK_IMAGE_TYPE_3D;

    if(extent.height > 1)
        return VK_IMAGE_TYPE_2D;

    return VK_IMAGE_TYPE_1D;
}

VkImageViewType vulkan_helpers::find_image_view_type(VkImageType image_type, bool array, bool cube)
{
    switch(image_type)
    {
        case VK_IMAGE_TYPE_1D:
        {
            return (array ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D);
        }
        case VK_IMAGE_TYPE_2D:
        {
            return (array ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D);
        }
        case VK_IMAGE_TYPE_3D:
        {
            return (cube && array ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : (cube ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_3D));
        }
        case VK_IMAGE_TYPE_RANGE_SIZE:
        case VK_IMAGE_TYPE_MAX_ENUM:
            return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }

    return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

VkImageAspectFlags vulkan_helpers::find_image_aspects(VkFormat format)
{
    VkImageAspectFlags flags = 0;
    bool is_depth_format   = depth_formats.find(format) != depth_formats.end();
    bool is_stencil_format = stencil_formats.find(format) != stencil_formats.end();

    if(!is_depth_format && !is_stencil_format)
    {
        flags |= VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else
    {
        if(is_depth_format)
            flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        if(is_stencil_format)
            flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    return flags;
}

VkFormat vulkan_helpers::convert_format(Format format)
{
    switch(format)
    {
        case Format::R8G8B8A8: return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::B8G8R8A8: return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::R16G16B16A16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case Format::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
        case Format::D24_S8: return VK_FORMAT_D24_UNORM_S8_UINT;
    }

    return VK_FORMAT_MAX_ENUM;
}

VkShaderStageFlags vulkan_helpers::convert_shader_stage(ShaderType type)
{
    switch(type)
    {
        case ShaderType::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderType::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

VkImageLayout vulkan_helpers::convert_image_layout(ImageLayout layout)
{
    switch(layout)
    {
        case ImageLayout::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
        case ImageLayout::GENERAL: return VK_IMAGE_LAYOUT_GENERAL;
        case ImageLayout::COLOUR_ATTACHMENT: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ImageLayout::DEPTH_STENCIL_ATTACHMENT: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case ImageLayout::SHADER_READ_ONLY: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageLayout::PRESENT: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkAttachmentLoadOp vulkan_helpers::convert_load_op(LoadOp op)
{
    switch(op)
    {
        case LoadOp::LOAD: return VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOp::CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOp::DONT_CARE: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

VkAttachmentStoreOp vulkan_helpers::convert_store_op(StoreOp op)
{
    switch(op)
    {
        case StoreOp::STORE: return VK_ATTACHMENT_STORE_OP_STORE;
        case StoreOp::DONT_CARE: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }

    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

VkAccessFlagBits vulkan_helpers::convert_memory_access(MemoryAccess access)
{
    switch(access)
    {
        case MemoryAccess::NO_ACCESS:                      return static_cast<VkAccessFlagBits>(0);
        case MemoryAccess::INDIRECT_COMMAND_READ:          return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        case MemoryAccess::INDEX_READ:                     return VK_ACCESS_INDEX_READ_BIT;
        case MemoryAccess::VERTEX_ATTRIBUTE_READ:          return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        case MemoryAccess::UNIFORM_READ:                   return VK_ACCESS_UNIFORM_READ_BIT;
        case MemoryAccess::INPUT_ATTACHMENT_READ:          return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        case MemoryAccess::SHADER_READ:                    return VK_ACCESS_SHADER_READ_BIT;
        case MemoryAccess::SHADER_WRITE:                   return VK_ACCESS_SHADER_WRITE_BIT;
        case MemoryAccess::COLOUR_ATTACHMENT_READ:         return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        case MemoryAccess::COLOUR_ATTACHMENT_WRITE:        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case MemoryAccess::DEPTH_STENCIL_ATTACHMENT_READ:  return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        case MemoryAccess::DEPTH_STENCIL_ATTACHMENT_WRITE: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case MemoryAccess::TRANSFER_READ:                  return VK_ACCESS_TRANSFER_READ_BIT;
        case MemoryAccess::TRANSFER_WRITE:                 return VK_ACCESS_TRANSFER_WRITE_BIT;
        case MemoryAccess::HOST_READ:                      return VK_ACCESS_HOST_READ_BIT;
        case MemoryAccess::HOST_WRITE:                     return VK_ACCESS_HOST_WRITE_BIT;
        case MemoryAccess::MEMORY_READ:                    return VK_ACCESS_MEMORY_READ_BIT;
        case MemoryAccess::MEMORY_WRITE:                   return VK_ACCESS_MEMORY_WRITE_BIT;
    }

    return static_cast<VkAccessFlagBits>(0);
}

VkAccessFlags vulkan_helpers::convert_memory_accesses(MemoryAccesses accesses)
{
    VkAccessFlags ret_flags = 0;
    uint32_t flag_check = 1;

    while(flag_check)
    {
        ret_flags |= convert_memory_access(static_cast<MemoryAccess>(accesses & flag_check));
        flag_check <<= 1;
    }

    return ret_flags;
}

VkPipelineStageFlagBits vulkan_helpers::convert_pipeline_stage(PipelineStage stage)
{
    switch(stage)
    {
        case PipelineStage::NO_STAGE:                return static_cast<VkPipelineStageFlagBits>(0);
        case PipelineStage::TOP_OF_PIPE:             return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        case PipelineStage::DRAW_INDIRECT:           return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        case PipelineStage::VERTEX_INPUT:            return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        case PipelineStage::VERTEX_SHADER:           return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        case PipelineStage::TESSELLATION_CONTROL:    return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
        case PipelineStage::TESSELLATION_EVALUATION: return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
        case PipelineStage::GEOMETRY_SHADER:         return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
        case PipelineStage::TRANSFORM_FEEDBACK:      return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
        case PipelineStage::EARLY_FRAGMENT_TEST:     return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case PipelineStage::FRAGMENT_SHADER:         return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case PipelineStage::LATE_FRAGMENT_TEST:      return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        case PipelineStage::COLOUR_OUTPUT:           return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case PipelineStage::BOTTOM_OF_PIPE:          return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }

    return static_cast<VkPipelineStageFlagBits>(0);
}

VkPipelineStageFlags vulkan_helpers::convert_pipeline_stages(PipelineStages stages)
{
    VkPipelineStageFlags ret_flags = 0;
    uint32_t flag_check = 1;

    while(flag_check)
    {
        ret_flags |= convert_pipeline_stage(static_cast<PipelineStage>(stages & flag_check));
        flag_check <<= 1;
    }

    return ret_flags;
}

VkPrimitiveTopology vulkan_helpers::convert_topology(Topology topology)
{
    switch(topology)
    {
        case Topology::TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}
