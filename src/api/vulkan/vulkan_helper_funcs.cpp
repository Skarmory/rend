#include "api/vulkan/vulkan_helper_funcs.h"

#include "api/vulkan/swapchain.h"
#include "api/vulkan/vulkan_renderer.h"
#include "core/renderer.h"

#include <cassert>
#include <iostream>
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

VkAttachmentDescription vulkan_helpers::convert_attachment_description(const AttachmentInfo& info)
{
    return
    VkAttachmentDescription{
        0,
        convert_format(info.format),
        convert_sample_count(info.samples),
        convert_load_op(info.load_op),
        convert_store_op(info.store_op),
        convert_load_op(info.stencil_load_op),
        convert_store_op(info.stencil_store_op),
        convert_image_layout(info.initial_layout),
        convert_image_layout(info.final_layout)
    };
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
        case Format::SWAPCHAIN:
        {
            rend::VulkanRenderer& rr = static_cast<rend::VulkanRenderer&>(rend::Renderer::get_instance());
            return convert_format(rr.get_swapchain()->get_format());
        }
    }

    return VK_FORMAT_MAX_ENUM;
}

VkShaderStageFlagBits vulkan_helpers::convert_shader_stage(ShaderStage type)
{
    switch(type)
    {
        case ShaderStage::SHADER_STAGE_VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::SHADER_STAGE_FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::SHADER_STAGE_TESSELLATION_CONTROL: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage::SHADER_STAGE_TESSELLATION_EVALUATION: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case ShaderStage::SHADER_STAGE_GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::SHADER_STAGE_COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        case ShaderStage::SHADER_STAGE_NONE: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }

    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

VkShaderStageFlags vulkan_helpers::convert_shader_stages(ShaderStages stages)
{
    VkShaderStageFlags ret_flags = 0;
    uint32_t flag_check = 1;

    while(flag_check)
    {
        ret_flags |= convert_shader_stage(static_cast<ShaderStage>(stages & flag_check));
        flag_check <<= 1;
    }

    return ret_flags;
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
        case ImageLayout::TRANSFER_SRC: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ImageLayout::TRANSFER_DST: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
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

VkPipelineBindPoint vulkan_helpers::convert_pipeline_bind_point(PipelineBindPoint bind_point)
{
    switch(bind_point)
    {
        case PipelineBindPoint::GRAPHICS: return VK_PIPELINE_BIND_POINT_GRAPHICS;
        case PipelineBindPoint::COMPUTE: return VK_PIPELINE_BIND_POINT_COMPUTE;
    }

    return VK_PIPELINE_BIND_POINT_GRAPHICS;
}

VkPipelineStageFlagBits vulkan_helpers::convert_pipeline_stage(PipelineStage stage)
{
    switch(stage)
    {
        case PipelineStage::PIPELINE_STAGE_NONE:                    return static_cast<VkPipelineStageFlagBits>(0);
        case PipelineStage::PIPELINE_STAGE_TOP_OF_PIPE:             return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        case PipelineStage::PIPELINE_STAGE_DRAW_INDIRECT:           return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        case PipelineStage::PIPELINE_STAGE_VERTEX_INPUT:            return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        case PipelineStage::PIPELINE_STAGE_VERTEX_SHADER:           return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        case PipelineStage::PIPELINE_STAGE_TESSELLATION_CONTROL:    return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
        case PipelineStage::PIPELINE_STAGE_TESSELLATION_EVALUATION: return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
        case PipelineStage::PIPELINE_STAGE_GEOMETRY_SHADER:         return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
        case PipelineStage::PIPELINE_STAGE_TRANSFORM_FEEDBACK:      return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
        case PipelineStage::PIPELINE_STAGE_EARLY_FRAGMENT_TEST:     return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case PipelineStage::PIPELINE_STAGE_FRAGMENT_SHADER:         return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case PipelineStage::PIPELINE_STAGE_LATE_FRAGMENT_TEST:      return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        case PipelineStage::PIPELINE_STAGE_COLOUR_OUTPUT:           return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case PipelineStage::PIPELINE_STAGE_TRANSFER:                return VK_PIPELINE_STAGE_TRANSFER_BIT;
        case PipelineStage::PIPELINE_STAGE_BOTTOM_OF_PIPE:          return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
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

VkPushConstantRange vulkan_helpers::convert_push_constant_range(PushConstantRange push_constant_range)
{
    VkPushConstantRange vk_push_constant_range{};
    vk_push_constant_range.stageFlags = convert_shader_stages(push_constant_range.shader_stages);
    vk_push_constant_range.offset     = push_constant_range.offset;
    vk_push_constant_range.size       = push_constant_range.size;

    return vk_push_constant_range;
}

VkPrimitiveTopology vulkan_helpers::convert_topology(Topology topology)
{
    switch(topology)
    {
        case Topology::TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

VkPolygonMode vulkan_helpers::convert_polygon_mode(PolygonMode mode)
{
    switch(mode)
    {
        case PolygonMode::FILL: return VK_POLYGON_MODE_FILL;
        case PolygonMode::LINE: return VK_POLYGON_MODE_LINE;
        case PolygonMode::POINT: return VK_POLYGON_MODE_POINT;
    }

    return VK_POLYGON_MODE_MAX_ENUM;
}

VkFrontFace vulkan_helpers::convert_front_face(FrontFace face)
{
    switch(face)
    {
        case FrontFace::CW:  return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CCW: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    return VK_FRONT_FACE_MAX_ENUM;
}

VkCullModeFlagBits vulkan_helpers::convert_cull_mode(CullMode mode)
{
    switch(mode)
    {
        case CullMode::NONE: return VK_CULL_MODE_NONE;
        case CullMode::FRONT: return VK_CULL_MODE_FRONT_BIT;
        case CullMode::BACK: return VK_CULL_MODE_BACK_BIT;
        case CullMode::FRONT_AND_BACK: return VK_CULL_MODE_FRONT_AND_BACK;
    }

    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
}

VkSampleCountFlagBits vulkan_helpers::convert_sample_count(MSAASamples samples)
{
    switch(samples)
    {
        case MSAASamples::MSAA_1X: return VK_SAMPLE_COUNT_1_BIT;
        case MSAASamples::MSAA_2X: return VK_SAMPLE_COUNT_2_BIT;
        case MSAASamples::MSAA_4X: return VK_SAMPLE_COUNT_4_BIT;
        case MSAASamples::MSAA_8X: return VK_SAMPLE_COUNT_8_BIT;
        case MSAASamples::MSAA_16X: return VK_SAMPLE_COUNT_16_BIT;
        case MSAASamples::MSAA_32X: return VK_SAMPLE_COUNT_32_BIT;
        case MSAASamples::MSAA_64X: return VK_SAMPLE_COUNT_64_BIT;
    }

    return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

VkCompareOp vulkan_helpers::convert_compare_op(CompareOp compare_op)
{
    switch(compare_op)
    {
        case CompareOp::NEVER: return VK_COMPARE_OP_NEVER;
        case CompareOp::LESS: return VK_COMPARE_OP_LESS;
        case CompareOp::EQUAL: return VK_COMPARE_OP_EQUAL;
        case CompareOp::LESS_OR_EQUAL: return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::GREATER: return VK_COMPARE_OP_GREATER;
        case CompareOp::NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GREATER_OR_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::ALWAYS: return VK_COMPARE_OP_ALWAYS;
    }

    return VK_COMPARE_OP_MAX_ENUM;
}

VkStencilOp vulkan_helpers::convert_stencil_op(StencilOp stencil_op)
{
    switch(stencil_op)
    {
        case StencilOp::KEEP: return VK_STENCIL_OP_KEEP;
        case StencilOp::ZERO: return VK_STENCIL_OP_ZERO;
        case StencilOp::REPLACE: return VK_STENCIL_OP_REPLACE;
        case StencilOp::INCREMENT_AND_CLAMP: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOp::DECREMENT_AND_CLAMP: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOp::INVERT: return VK_STENCIL_OP_INVERT;
        case StencilOp::INCREMENT_AND_WRAP: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOp::DECREMENT_AND_WRAP: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }

    return VK_STENCIL_OP_MAX_ENUM;
}

VkLogicOp vulkan_helpers::convert_logic_op(LogicOp logic_op)
{
    switch(logic_op)
    {
        case LogicOp::CLEAR: return VK_LOGIC_OP_CLEAR;
        case LogicOp::AND: return VK_LOGIC_OP_AND;
        case LogicOp::AND_REVERSE: return VK_LOGIC_OP_AND_REVERSE;
        case LogicOp::COPY: return VK_LOGIC_OP_COPY;
        case LogicOp::AND_INVERTED: return VK_LOGIC_OP_AND_INVERTED;
        case LogicOp::NO_OP: return VK_LOGIC_OP_NO_OP;
        case LogicOp::XOR: return VK_LOGIC_OP_XOR;
        case LogicOp::OR: return VK_LOGIC_OP_OR;
        case LogicOp::NOR: return VK_LOGIC_OP_NOR;
        case LogicOp::EQUIVALENT: return VK_LOGIC_OP_EQUIVALENT;
        case LogicOp::INVERT: return VK_LOGIC_OP_INVERT;
        case LogicOp::OR_REVERSE: return VK_LOGIC_OP_OR_REVERSE;
        case LogicOp::COPY_INVERTED: return VK_LOGIC_OP_COPY_INVERTED;
        case LogicOp::OR_INVERTED: return VK_LOGIC_OP_OR_INVERTED;
        case LogicOp::NAND: return VK_LOGIC_OP_NAND;
        case LogicOp::SET: return VK_LOGIC_OP_SET;
    }

    return VK_LOGIC_OP_MAX_ENUM;
}

VkBlendFactor vulkan_helpers::convert_blend_factor(BlendFactor factor)
{
    switch(factor)
    {
        case BlendFactor::ZERO: return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::ONE: return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SRC_COLOUR: return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOUR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DST_COLOUR: return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOUR: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA: return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DST_ALPHA: return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::CONSTANT_COLOUR: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOUR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::CONSTANT_ALPHA: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SRC_ALPHA_SATURATE: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case BlendFactor::SRC1_COLOUR: return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOUR: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA: return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    }

    return VK_BLEND_FACTOR_MAX_ENUM;
}

VkBlendOp vulkan_helpers::convert_blend_op(BlendOp op)
{
    switch(op)
    {
        case BlendOp::ADD: return VK_BLEND_OP_ADD;
        case BlendOp::SUBTRACT: return VK_BLEND_OP_SUBTRACT;
        case BlendOp::REVERSE_SUBTRACT: return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::MIN: return VK_BLEND_OP_MIN;
        case BlendOp::MAX: return VK_BLEND_OP_MAX;
    }

    return VK_BLEND_OP_MAX_ENUM;
}

VkDynamicState vulkan_helpers::convert_dynamic_state(DynamicState state)
{
    switch(state)
    {
        case DynamicState::VIEWPORT: return VK_DYNAMIC_STATE_VIEWPORT;
        case DynamicState::SCISSOR: return VK_DYNAMIC_STATE_SCISSOR;
        case DynamicState::LINE_WIDTH: return VK_DYNAMIC_STATE_LINE_WIDTH;
        case DynamicState::DEPTH_BIAS: return VK_DYNAMIC_STATE_DEPTH_BIAS;
        case DynamicState::BLEND_CONSTANTS: return VK_DYNAMIC_STATE_BLEND_CONSTANTS;
        case DynamicState::DEPTH_BOUNDS: return VK_DYNAMIC_STATE_DEPTH_BOUNDS;
        case DynamicState::STENCIL_COMPARE_MASK: return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
        case DynamicState::STENCIL_WRITE_MASK: return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
        case DynamicState::STENCIL_REFERENCE: return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
    };

    return VK_DYNAMIC_STATE_MAX_ENUM;
}

ImageLayout vulkan_helpers::convert_image_layout(VkImageLayout layout)
{
    switch(layout)
    {
        case VK_IMAGE_LAYOUT_UNDEFINED: return ImageLayout::UNDEFINED;
        case VK_IMAGE_LAYOUT_GENERAL: return ImageLayout::GENERAL;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return ImageLayout::COLOUR_ATTACHMENT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return ImageLayout::DEPTH_STENCIL_ATTACHMENT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return ImageLayout::SHADER_READ_ONLY;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return ImageLayout::TRANSFER_SRC;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return ImageLayout::TRANSFER_DST;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return ImageLayout::PRESENT;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
        case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV:
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR:
        case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR:
            std::cerr << "Unsupported VkImageLayout (" << stringify(layout) << ") for conversion" << std::endl;
            assert("Invalid conversion" || true);
            return ImageLayout::UNDEFINED;
        case VK_IMAGE_LAYOUT_MAX_ENUM:
            std::cerr << "Invalid VkImageLayout (" << stringify(layout) << ") for conversion" << std::endl;
            assert("Invalid conversion" || true);
            return ImageLayout::UNDEFINED;
    }

    return ImageLayout::UNDEFINED;
}

uint32_t vulkan_helpers::convert_sample_count(VkSampleCountFlagBits samples)
{
    switch(samples)
    {
        case VK_SAMPLE_COUNT_1_BIT: return 1;
        case VK_SAMPLE_COUNT_2_BIT: return 2;
        case VK_SAMPLE_COUNT_4_BIT: return 4;
        case VK_SAMPLE_COUNT_8_BIT: return 8;
        case VK_SAMPLE_COUNT_16_BIT: return 16;
        case VK_SAMPLE_COUNT_32_BIT: return 32;
        case VK_SAMPLE_COUNT_64_BIT: return 64;
        case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM:
            std::cerr << "Invalid VkSampleCountFlagBits (" << stringify(samples) << ") for conversion" << std::endl;
            assert("Invalid conversion" || true);
    }

    return 1;
}

Format vulkan_helpers::convert_format(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8G8B8A8_UNORM: return Format::R8G8B8A8;
        case VK_FORMAT_B8G8R8A8_UNORM: return Format::B8G8R8A8;
        case VK_FORMAT_R16G16B16A16_SFLOAT: return Format::R16G16B16A16_SFLOAT;
        case VK_FORMAT_R32G32B32_SFLOAT: return Format::R32G32B32_SFLOAT;
        case VK_FORMAT_R32G32_SFLOAT: return Format::R32G32_SFLOAT;
        case VK_FORMAT_D24_UNORM_S8_UINT: return Format::D24_S8;
        default:
            std::cerr << "Unsupported VkFormat passed for conversion: enum number " << format << std::endl;
            assert("Unsupported VkFormat" || true);
    }

    return Format::R8G8B8A8;
}

VkBufferUsageFlags vulkan_helpers::convert_buffer_usage_flags(BufferUsage usage)
{
    VkImageUsageFlags ret{ 0 };
    uint32_t value{ (uint32_t)usage };
    uint32_t check{ 1 };

    while (value != 0 && check != 0)
    {
        if (value & check)
        {
            switch (static_cast<BufferUsage>(check))
            {
                case BufferUsage::TRANSFER_SRC: ret |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT; break;
                case BufferUsage::TRANSFER_DST: ret |= VK_BUFFER_USAGE_TRANSFER_DST_BIT; break;
                case BufferUsage::VERTEX_BUFFER: ret |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
                case BufferUsage::INDEX_BUFFER: ret |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
                case BufferUsage::UNIFORM_BUFFER: ret |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
                case BufferUsage::NONE:
                default:
                    break;
            }
        }

        check = check << 1;
    }

    return ret;
}

VkImageUsageFlags vulkan_helpers::convert_image_usage_flags(ImageUsage usage)
{
    VkImageUsageFlags ret{ 0 };
    ImageUsage check{ 1 };

    while (usage != ImageUsage::NONE && check != ImageUsage::NONE)
    {
        if ((usage & check) != ImageUsage::NONE)
        {
            switch (check)
            {
                case ImageUsage::TRANSFER_SRC: ret |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; break;
                case ImageUsage::TRANSFER_DST: ret |= VK_IMAGE_USAGE_TRANSFER_DST_BIT; break;
                case ImageUsage::SAMPLED: ret |= VK_IMAGE_USAGE_SAMPLED_BIT; break;
                case ImageUsage::DEPTH_STENCIL: ret |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; break;
                case ImageUsage::COLOUR_ATTACHMENT: ret |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; break;
                case ImageUsage::NONE:
                default:
                    break;
            }
        }

        check = check << 1;
    }

    return ret;
}

VkDescriptorType vulkan_helpers::convert_descriptor_type(DescriptorType type)
{
    switch(type)
    {
        case DescriptorType::SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case DescriptorType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorType::SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case DescriptorType::STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case DescriptorType::UNIFORM_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        case DescriptorType::STORAGE_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case DescriptorType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorType::STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case DescriptorType::UNIFORM_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case DescriptorType::STORAGE_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        case DescriptorType::INPUT_ATTACHMENT: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    }

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkImageCopy vulkan_helpers::convert_image_copy(const ImageImageCopyInfo& copy)
{
    VkImageCopy vk_copy =
    {
        .srcSubresource = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = copy.mip_level, .baseArrayLayer = copy.base_layer, .layerCount = copy.layer_count  },
        .srcOffset = { .x = copy.src_offset_x, .y = copy.src_offset_y, .z = copy.src_offset_z },
        .dstSubresource = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = copy.mip_level, .baseArrayLayer = copy.base_layer, .layerCount = copy.layer_count  },
        .dstOffset = { .x = copy.dst_offset_x, .y = copy.dst_offset_y, .z = copy.dst_offset_z },
        .extent = { .width = copy.extent_x, .height = copy.extent_y, .depth = copy.extent_z }
    };

    return vk_copy;
}

VkVertexInputAttributeDescription vulkan_helpers::convert_vertex_attribute_info(const VertexAttributeInfo& info, int binding)
{
    VkVertexInputAttributeDescription vk_vertex_attribute =
    {
        .location = info.location,
        .binding = binding,
        .format = vulkan_helpers::convert_format(info.format),
        .offset = info.location * info.align
    };

    return vk_vertex_attribute;
}

VkVertexInputBindingDescription vulkan_helpers::convert_vertex_binding_info(const VertexBindingInfo& info)
{
    VkVertexInputBindingDescription vk_vertex_binding = 
    {
        .binding = info.index,
        .stride = info.stride,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    return vk_vertex_binding;
}

VkMemoryAllocateInfo vulkan_helpers::gen_memory_allocate_info(void)
{
    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = nullptr;

    return info;
}

VkSamplerCreateInfo vulkan_helpers::gen_sampler_create_info(void)
{
    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkImageCreateInfo vulkan_helpers::gen_image_create_info(void)
{
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkImageViewCreateInfo vulkan_helpers::gen_image_view_create_info(void)
{
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkBufferCreateInfo vulkan_helpers::gen_buffer_create_info(void)
{
    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkShaderModuleCreateInfo vulkan_helpers::gen_shader_module_create_info(void)
{
    VkShaderModuleCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPhysicalDeviceFeatures vulkan_helpers::gen_vk_1_0_features(void)
{
    VkPhysicalDeviceFeatures vk_1_0_features =
    {
        .robustBufferAccess = VK_FALSE,
        .fullDrawIndexUint32 = VK_FALSE,
        .imageCubeArray = VK_FALSE,
        .independentBlend = VK_FALSE,
        .geometryShader = VK_FALSE,
        .tessellationShader = VK_FALSE,
        .sampleRateShading = VK_FALSE,
        .dualSrcBlend = VK_FALSE,
        .logicOp = VK_FALSE,
        .multiDrawIndirect = VK_FALSE,
        .drawIndirectFirstInstance = VK_FALSE,
        .depthClamp = VK_FALSE,
        .depthBiasClamp = VK_FALSE,
        .fillModeNonSolid = VK_FALSE,
        .depthBounds = VK_FALSE,
        .wideLines = VK_FALSE,
        .largePoints = VK_FALSE,
        .alphaToOne = VK_FALSE,
        .multiViewport = VK_FALSE,
        .samplerAnisotropy = VK_FALSE,
        .textureCompressionETC2 = VK_FALSE,
        .textureCompressionASTC_LDR = VK_FALSE,
        .textureCompressionBC = VK_FALSE,
        .occlusionQueryPrecise = VK_FALSE,
        .pipelineStatisticsQuery = VK_FALSE,
        .vertexPipelineStoresAndAtomics = VK_FALSE,
        .fragmentStoresAndAtomics = VK_FALSE,
        .shaderTessellationAndGeometryPointSize = VK_FALSE,
        .shaderImageGatherExtended = VK_FALSE,
        .shaderStorageImageExtendedFormats = VK_FALSE,
        .shaderStorageImageMultisample = VK_FALSE,
        .shaderStorageImageReadWithoutFormat = VK_FALSE,
        .shaderStorageImageWriteWithoutFormat = VK_FALSE,
        .shaderUniformBufferArrayDynamicIndexing = VK_FALSE,
        .shaderSampledImageArrayDynamicIndexing = VK_FALSE,
        .shaderStorageBufferArrayDynamicIndexing = VK_FALSE,
        .shaderStorageImageArrayDynamicIndexing = VK_FALSE,
        .shaderClipDistance = VK_FALSE,
        .shaderCullDistance = VK_FALSE,
        .shaderFloat64 = VK_FALSE,
        .shaderInt64 = VK_FALSE,
        .shaderInt16 = VK_FALSE,
        .shaderResourceResidency = VK_FALSE,
        .shaderResourceMinLod = VK_FALSE,
        .sparseBinding = VK_FALSE,
        .sparseResidencyBuffer = VK_FALSE,
        .sparseResidencyImage2D = VK_FALSE,
        .sparseResidencyImage3D = VK_FALSE,
        .sparseResidency2Samples = VK_FALSE,
        .sparseResidency4Samples = VK_FALSE,
        .sparseResidency8Samples = VK_FALSE,
        .sparseResidency16Samples = VK_FALSE,
        .sparseResidencyAliased = VK_FALSE,
        .variableMultisampleRate = VK_FALSE,
        .inheritedQueries = VK_FALSE
    };

    return vk_1_0_features;
}

VkPhysicalDeviceVulkan11Features vulkan_helpers::gen_vk_1_1_features(void)
{
    VkPhysicalDeviceVulkan11Features vk_1_1_features =
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = nullptr,
        .storageBuffer16BitAccess = VK_FALSE,
        .uniformAndStorageBuffer16BitAccess = VK_FALSE,
        .storagePushConstant16 = VK_FALSE,
        .storageInputOutput16 = VK_FALSE,
        .multiview = VK_FALSE,
        .multiviewGeometryShader = VK_FALSE,
        .multiviewTessellationShader = VK_FALSE,
        .variablePointersStorageBuffer = VK_FALSE,
        .variablePointers = VK_FALSE,
        .protectedMemory = VK_FALSE,
        .samplerYcbcrConversion = VK_FALSE,
        .shaderDrawParameters = VK_FALSE
    };

    return vk_1_1_features;
}

VkPhysicalDeviceVulkan12Features vulkan_helpers::gen_vk_1_2_features(void)
{
    VkPhysicalDeviceVulkan12Features vk_1_2_features = 
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = nullptr,
        .samplerMirrorClampToEdge = VK_FALSE,
        .drawIndirectCount = VK_FALSE,
        .storageBuffer8BitAccess = VK_FALSE,
        .uniformAndStorageBuffer8BitAccess = VK_FALSE,
        .storagePushConstant8 = VK_FALSE,
        .shaderBufferInt64Atomics = VK_FALSE,
        .shaderSharedInt64Atomics = VK_FALSE,
        .shaderFloat16 = VK_FALSE,
        .shaderInt8 = VK_FALSE,
        .descriptorIndexing = VK_FALSE,
        .shaderInputAttachmentArrayDynamicIndexing = VK_FALSE,
        .shaderUniformTexelBufferArrayDynamicIndexing = VK_FALSE,
        .shaderStorageTexelBufferArrayDynamicIndexing = VK_FALSE,
        .shaderUniformBufferArrayNonUniformIndexing = VK_FALSE,
        .shaderSampledImageArrayNonUniformIndexing = VK_FALSE,
        .shaderStorageBufferArrayNonUniformIndexing = VK_FALSE,
        .shaderStorageImageArrayNonUniformIndexing = VK_FALSE,
        .shaderInputAttachmentArrayNonUniformIndexing = VK_FALSE,
        .shaderUniformTexelBufferArrayNonUniformIndexing = VK_FALSE,
        .shaderStorageTexelBufferArrayNonUniformIndexing = VK_FALSE,
        .descriptorBindingUniformBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingSampledImageUpdateAfterBind = VK_FALSE,
        .descriptorBindingStorageImageUpdateAfterBind = VK_FALSE,
        .descriptorBindingStorageBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingUniformTexelBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingStorageTexelBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingUpdateUnusedWhilePending = VK_FALSE,
        .descriptorBindingPartiallyBound = VK_FALSE,
        .descriptorBindingVariableDescriptorCount = VK_FALSE,
        .runtimeDescriptorArray = VK_FALSE,
        .samplerFilterMinmax = VK_FALSE,
        .scalarBlockLayout = VK_FALSE,
        .imagelessFramebuffer = VK_FALSE,
        .uniformBufferStandardLayout = VK_FALSE,
        .shaderSubgroupExtendedTypes = VK_FALSE,
        .separateDepthStencilLayouts = VK_FALSE,
        .hostQueryReset = VK_FALSE,
        .timelineSemaphore = VK_FALSE,
        .bufferDeviceAddress = VK_FALSE,
        .bufferDeviceAddressCaptureReplay = VK_FALSE,
        .bufferDeviceAddressMultiDevice = VK_FALSE,
        .vulkanMemoryModel = VK_FALSE,
        .vulkanMemoryModelDeviceScope = VK_FALSE,
        .vulkanMemoryModelAvailabilityVisibilityChains = VK_FALSE,
        .shaderOutputViewportIndex = VK_FALSE,
        .shaderOutputLayer = VK_FALSE,
        .subgroupBroadcastDynamicId = VK_FALSE
    };

    return vk_1_2_features;
}

VkPipelineLayoutCreateInfo vulkan_helpers::gen_pipeline_layout_create_info(void)
{
    VkPipelineLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineShaderStageCreateInfo vulkan_helpers::gen_shader_stage_create_info(void)
{
    VkPipelineShaderStageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineVertexInputStateCreateInfo vulkan_helpers::gen_vertex_input_state_create_info(void)
{
    VkPipelineVertexInputStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineInputAssemblyStateCreateInfo vulkan_helpers::gen_input_assembly_state_create_info(void)
{
    VkPipelineInputAssemblyStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineTessellationStateCreateInfo vulkan_helpers::gen_tessellation_state_create_info(void)
{
    VkPipelineTessellationStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineViewportStateCreateInfo vulkan_helpers::gen_viewport_state_create_info(void)
{
    VkPipelineViewportStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineRasterizationStateCreateInfo vulkan_helpers::gen_rasterisation_state_create_info(void)
{
    VkPipelineRasterizationStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineMultisampleStateCreateInfo vulkan_helpers::gen_multisample_state_create_info(void)
{
    VkPipelineMultisampleStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineDepthStencilStateCreateInfo vulkan_helpers::gen_depth_stencil_state_create_info(void)
{
    VkPipelineDepthStencilStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineColorBlendStateCreateInfo vulkan_helpers::gen_colour_blend_state_create_info(void)
{
    VkPipelineColorBlendStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkPipelineDynamicStateCreateInfo vulkan_helpers::gen_dynamic_state_create_info(void)
{
    VkPipelineDynamicStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkGraphicsPipelineCreateInfo vulkan_helpers::gen_graphics_pipeline_create_info(void)
{
    VkGraphicsPipelineCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkCommandPoolCreateInfo vulkan_helpers::gen_command_pool_create_info(void)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkEventCreateInfo vulkan_helpers::gen_event_create_info(void)
{
    VkEventCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkFenceCreateInfo vulkan_helpers::gen_fence_create_info(void)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkSemaphoreCreateInfo vulkan_helpers::gen_semaphore_create_info(void)
{
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkDescriptorSetLayoutCreateInfo vulkan_helpers::gen_descriptor_set_layout_create_info(void)
{
    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkDescriptorPoolCreateInfo vulkan_helpers::gen_descriptor_pool_create_info(void)
{
    VkDescriptorPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkFramebufferCreateInfo vulkan_helpers::gen_framebuffer_create_info(void)
{
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkRenderPassCreateInfo vulkan_helpers::gen_render_pass_create_info(void)
{
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkSwapchainCreateInfoKHR vulkan_helpers::gen_swapchain_create_info(void)
{
    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

const char* vulkan_helpers::stringify(VkImageLayout layout)
{
    switch(layout)
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:                                  return "VK_IMAGE_LAYOUT_UNDEFINED";
        case VK_IMAGE_LAYOUT_GENERAL:                                    return "VK_IMAGE_LAYOUT_GENERAL";
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:                   return "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:           return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:            return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:                   return "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:                       return "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL";
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:                       return "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL";
        case VK_IMAGE_LAYOUT_PREINITIALIZED:                             return "VK_IMAGE_LAYOUT_PREINITIALIZED";
        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:                            return "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
        case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:                         return "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR";
        case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV:                    return "VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV";
        case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:           return "VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT";
        case VK_IMAGE_LAYOUT_MAX_ENUM:                                   return "VK_IMAGE_LAYOUT_MAX_ENUM";
        default:                                                         return "Unknown VkImageLayout";
    }
}

const char* vulkan_helpers::stringify(VkSampleCountFlagBits sample_count)
{
    switch(sample_count)
    {
        case VK_SAMPLE_COUNT_1_BIT:              return "VK_SAMPLE_COUNT_1_BIT";
        case VK_SAMPLE_COUNT_2_BIT:              return "VK_SAMPLE_COUNT_2_BIT";
        case VK_SAMPLE_COUNT_4_BIT:              return "VK_SAMPLE_COUNT_4_BIT";
        case VK_SAMPLE_COUNT_8_BIT:              return "VK_SAMPLE_COUNT_8_BIT";
        case VK_SAMPLE_COUNT_16_BIT:             return "VK_SAMPLE_COUNT_16_BIT";
        case VK_SAMPLE_COUNT_32_BIT:             return "VK_SAMPLE_COUNT_32_BIT";
        case VK_SAMPLE_COUNT_64_BIT:             return "VK_SAMPLE_COUNT_64_BIT";
        case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM: return "VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM";
        default:                                 return "Unknown VkSampleCountFlagBits";
    }
}

const char* vulkan_helpers::stringify(VkDescriptorType descriptor_type)
{
    switch(descriptor_type)
    {
        case VK_DESCRIPTOR_TYPE_SAMPLER:                   return "VK_DESCRIPTOR_TYPE_SAMPLER";
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:    return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:             return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:             return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
        case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:      return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
        case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:      return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:            return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:            return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:    return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:    return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:          return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
        case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT:  return "VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT";
        case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV: return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV";
        case VK_DESCRIPTOR_TYPE_MAX_ENUM:                  return "VK_DESCRIPTOR_TYPE_MAX_ENUM";
        default:                                           return "Unknown VkDescriptorType";
    }
}
