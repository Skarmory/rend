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

VkSampleCountFlagBits vulkan_helpers::convert_sample_count(uint32_t count)
{
    if(count <= 1)
        return VK_SAMPLE_COUNT_1_BIT;
    if(count <= 2)
        return VK_SAMPLE_COUNT_2_BIT;
    if(count <= 4)
        return VK_SAMPLE_COUNT_4_BIT;
    if(count <= 8)
        return VK_SAMPLE_COUNT_8_BIT;
    if(count <= 16)
        return VK_SAMPLE_COUNT_16_BIT;
    if(count <= 32)
        return VK_SAMPLE_COUNT_32_BIT;
    if(count <= 64)
        return VK_SAMPLE_COUNT_64_BIT;

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
