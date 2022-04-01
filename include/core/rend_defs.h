#ifndef REND_DEFS_H
#define REND_DEFS_H

#include "rend_constants.h"

#include <limits>
#include <cstdint>
#include <type_traits>

#define UU(x) ((void)x)
#define BIT(x) (static_cast<uint32_t>(1 << x))

namespace rend
{

typedef uint64_t HandleType;
typedef HandleType MemoryHandle;
typedef HandleType BufferHandle;
typedef HandleType TextureHandle;
typedef HandleType TextureViewHandle;
typedef HandleType SamplerHandle;
typedef HandleType ShaderHandle;
typedef HandleType FramebufferHandle;
typedef HandleType RenderPassHandle;
typedef HandleType PipelineHandle;
typedef HandleType CommandPoolHandle;
typedef HandleType CommandBufferHandle;
typedef HandleType DescriptorPoolHandle;
typedef HandleType DescriptorSetHandle;
typedef HandleType DescriptorSetLayoutHandle;
typedef HandleType PipelineLayoutHandle;
typedef HandleType PipelineHandle;
typedef BufferHandle VertexBufferHandle;
typedef BufferHandle IndexBufferHandle;
typedef BufferHandle UniformBufferHandle;
typedef TextureHandle Texture2DHandle;

constexpr HandleType NULL_HANDLE = std::numeric_limits<uint64_t>::max();

// Enums

enum class StatusCode
{
    // Generic
    SUCCESS,
    FAILURE,
    ALREADY_CREATED,
    RESOURCE_NOT_CREATED,

    // Instance
    INSTANCE_CREATE_FAILURE,

    // Device context
    CONTEXT_GPU_WITH_DESIRED_FEATURES_NOT_FOUND,
    CONTEXT_GPU_NOT_CHOSEN,
    CONTEXT_DEVICE_CREATE_FAILURE,

    // Swapchain
    SWAPCHAIN_OUT_OF_DATE,
    SWAPCHAIN_ACQUIRE_ERROR,
    SWAPCHAIN_NO_SURFACE_FORMATS_FOUND,
    SWAPCHAIN_NO_SURFACE_PRESENT_MODES_FOUND,

    // Image
    IMAGE_CREATE_FAILURE,
    IMAGE_VIEW_CREATE_FAILURE,
    SAMPLER_CREATE_FAILURE,

    // Memory
    MEMORY_ALLOC_FAILURE,
    MEMORY_BIND_IMAGE_FAILURE,

    // Window
    WINDOW_CREATE_FAILURE,
    WINDOW_NOT_CREATED
};

enum class Format
{
    R8G8B8A8,
    B8G8R8A8,
    R16G16B16A16_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32_SFLOAT,
    D24_S8
};

enum class MSAASamples
{
    MSAA_1X,
    MSAA_2X,
    MSAA_4X,
    MSAA_8X,
    MSAA_16X,
    MSAA_32X,
    MSAA_64X
};

enum ShaderStage
{
    SHADER_STAGE_NONE                    = 0,
    SHADER_STAGE_VERTEX                  = BIT(0),
    SHADER_STAGE_TESSELLATION_CONTROL    = BIT(1),
    SHADER_STAGE_TESSELLATION_EVALUATION = BIT(2),
    SHADER_STAGE_GEOMETRY                = BIT(3),
    SHADER_STAGE_FRAGMENT                = BIT(4),
    SHADER_STAGE_COMPUTE                 = BIT(5),
    SHADER_STAGE_COUNT                   = 6
};
typedef uint32_t ShaderStages;

enum class LoadOp
{
    LOAD,
    CLEAR,
    DONT_CARE
};

enum class StoreOp
{
    STORE,
    DONT_CARE
};

enum class ImageLayout
{
    UNDEFINED,
    GENERAL,
    COLOUR_ATTACHMENT,
    DEPTH_STENCIL_ATTACHMENT,
    SHADER_READ_ONLY,
    TRANSFER_SRC,
    TRANSFER_DST,
    PRESENT
};

enum PipelineStage
{
    PIPELINE_STAGE_NONE                    = 0,
    PIPELINE_STAGE_TOP_OF_PIPE             = BIT(0),
    PIPELINE_STAGE_DRAW_INDIRECT           = BIT(1),
    PIPELINE_STAGE_VERTEX_INPUT            = BIT(2),
    PIPELINE_STAGE_VERTEX_SHADER           = BIT(3),
    PIPELINE_STAGE_TESSELLATION_CONTROL    = BIT(4),
    PIPELINE_STAGE_TESSELLATION_EVALUATION = BIT(5),
    PIPELINE_STAGE_GEOMETRY_SHADER         = BIT(6),
    PIPELINE_STAGE_TRANSFORM_FEEDBACK      = BIT(7),
    PIPELINE_STAGE_EARLY_FRAGMENT_TEST     = BIT(8),
    PIPELINE_STAGE_FRAGMENT_SHADER         = BIT(9),
    PIPELINE_STAGE_LATE_FRAGMENT_TEST      = BIT(10),
    PIPELINE_STAGE_COLOUR_OUTPUT           = BIT(11),
    PIPELINE_STAGE_TRANSFER                = BIT(12),
    PIPELINE_STAGE_BOTTOM_OF_PIPE          = BIT(13)
};
typedef uint32_t PipelineStages;

enum class PipelineBindPoint
{
    GRAPHICS,
    COMPUTE
};

enum MemoryAccess
{
    NO_ACCESS                      = 0,
    INDIRECT_COMMAND_READ          = BIT(0),
    INDEX_READ                     = BIT(1),
    VERTEX_ATTRIBUTE_READ          = BIT(2),
    UNIFORM_READ                   = BIT(3),
    INPUT_ATTACHMENT_READ          = BIT(4),
    SHADER_READ                    = BIT(5),
    SHADER_WRITE                   = BIT(6),
    COLOUR_ATTACHMENT_READ         = BIT(7),
    COLOUR_ATTACHMENT_WRITE        = BIT(8),
    DEPTH_STENCIL_ATTACHMENT_READ  = BIT(9),
    DEPTH_STENCIL_ATTACHMENT_WRITE = BIT(11),
    TRANSFER_READ                  = BIT(12),
    TRANSFER_WRITE                 = BIT(13),
    HOST_READ                      = BIT(14),
    HOST_WRITE                     = BIT(15),
    MEMORY_READ                    = BIT(16),
    MEMORY_WRITE                   = BIT(17)
};
typedef uint32_t MemoryAccesses;

enum class Topology
{
    TRIANGLE_LIST
};

enum class PolygonMode
{
    FILL,
    LINE,
    POINT
};

enum class FrontFace
{
    CW,
    CCW
};

enum class CullMode
{
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK
};

enum class CompareOp
{
    NEVER,
    LESS,
    EQUAL,
    LESS_OR_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_OR_EQUAL,
    ALWAYS
};

enum class DescriptorType
{
    SAMPLER,
    COMBINED_IMAGE_SAMPLER,
    SAMPLED_IMAGE,
    STORAGE_IMAGE,
    UNIFORM_TEXEL_BUFFER,
    STORAGE_TEXEL_BUFFER,
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    UNIFORM_BUFFER_DYNAMIC,
    STORAGE_BUFFER_DYNAMIC,
    INPUT_ATTACHMENT
};

const uint32_t c_descriptor_types_count = static_cast<uint32_t>( DescriptorType::INPUT_ATTACHMENT ) + 1;

enum class StencilOp
{
    KEEP,
    ZERO,
    REPLACE,
    INCREMENT_AND_CLAMP,
    DECREMENT_AND_CLAMP,
    INVERT,
    INCREMENT_AND_WRAP,
    DECREMENT_AND_WRAP
};

enum class LogicOp
{
    CLEAR,
    AND,
    AND_REVERSE,
    COPY,
    AND_INVERTED,
    NO_OP,
    XOR,
    OR,
    NOR,
    EQUIVALENT,
    INVERT,
    OR_REVERSE,
    COPY_INVERTED,
    OR_INVERTED,
    NAND,
    SET
};

enum ColourComponent
{
    COLOUR_COMPONENT_R = BIT(0),
    COLOUR_COMPONENT_G = BIT(1),
    COLOUR_COMPONENT_B = BIT(2),
    COLOUR_COMPONENT_A = BIT(3)
};
typedef uint32_t ColourComponents;

enum class BlendFactor
{
    ZERO,
    ONE,
    SRC_COLOUR,
    ONE_MINUS_SRC_COLOUR,
    DST_COLOUR,
    ONE_MINUS_DST_COLOUR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    CONSTANT_COLOUR,
    ONE_MINUS_CONSTANT_COLOUR,
    CONSTANT_ALPHA,
    ONE_MINUS_CONSTANT_ALPHA,
    SRC_ALPHA_SATURATE,
    SRC1_COLOUR,
    ONE_MINUS_SRC1_COLOUR,
    SRC1_ALPHA,
    ONE_MINUS_SRC1_ALPHA
};

enum class BlendOp
{
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX
};

enum class DynamicState : uint32_t
{
    NONE                 = 0,
    VIEWPORT             = BIT(0),
    SCISSOR              = BIT(1),
    LINE_WIDTH           = BIT(2),
    DEPTH_BIAS           = BIT(3),
    BLEND_CONSTANTS      = BIT(4),
    DEPTH_BOUNDS         = BIT(5),
    STENCIL_COMPARE_MASK = BIT(6),
    STENCIL_WRITE_MASK   = BIT(7),
    STENCIL_REFERENCE    = BIT(8)
};
typedef DynamicState DynamicStates;

inline DynamicState operator|(DynamicState lhs, DynamicState rhs)
{
    using T = std::underlying_type_t<DynamicState>;
    return static_cast<DynamicState>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline DynamicState& operator|=(DynamicState& lhs, DynamicState rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

inline DynamicState operator&(DynamicState lhs, DynamicState rhs)
{
    using T = std::underlying_type_t<DynamicState>;
    return static_cast<DynamicState>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline DynamicState& operator&=(DynamicState& lhs, DynamicState rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

inline DynamicState operator<<(DynamicState lhs, int rhs)
{
    using T = std::underlying_type_t<DynamicState>;
    return static_cast<DynamicState>(static_cast<T>(lhs) << rhs);
}

enum class BufferUsage : uint32_t
{
    NONE           = 0,
    TRANSFER_SRC   = BIT(0),
    TRANSFER_DST   = BIT(1),
    VERTEX_BUFFER  = BIT(2),
    INDEX_BUFFER   = BIT(3),
    UNIFORM_BUFFER = BIT(4)
};

inline BufferUsage operator|(BufferUsage lhs, BufferUsage rhs)
{
    using T = std::underlying_type_t<BufferUsage>;
    return static_cast<BufferUsage>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline BufferUsage& operator|=(BufferUsage& lhs, BufferUsage rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

inline BufferUsage operator&(BufferUsage lhs, BufferUsage rhs)
{
    using T = std::underlying_type_t<BufferUsage>;
    return static_cast<BufferUsage>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline BufferUsage& operator&=(BufferUsage& lhs, BufferUsage rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

inline BufferUsage operator<<(BufferUsage lhs, int rhs)
{
    using T = std::underlying_type_t<BufferUsage>;
    return static_cast<BufferUsage>(static_cast<T>(lhs) << rhs);
}

enum class ImageUsage : uint32_t
{
    NONE              = 0,
    TRANSFER_SRC      = BIT(0),
    TRANSFER_DST      = BIT(1),
    SAMPLED           = BIT(2),
    DEPTH_STENCIL     = BIT(3),
    COLOUR_ATTACHMENT = BIT(4),
};

inline ImageUsage operator|(ImageUsage lhs, ImageUsage rhs)
{
    using T = std::underlying_type_t<ImageUsage>;
    return static_cast<ImageUsage>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline ImageUsage& operator|=(ImageUsage& lhs, ImageUsage rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

inline ImageUsage operator&(ImageUsage lhs, ImageUsage rhs)
{
    using T = std::underlying_type_t<ImageUsage>;
    return static_cast<ImageUsage>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline ImageUsage& operator&=(ImageUsage& lhs, ImageUsage rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

inline ImageUsage operator<<(ImageUsage lhs, int rhs)
{
    using T = std::underlying_type_t<ImageUsage>;
    return static_cast<ImageUsage>(static_cast<T>(lhs) << rhs);
}

// Data structs (no functions)

struct RGBA
{
    float r{ 0.0f };
    float g{ 0.0f };
    float b{ 0.0f };
    float a{ 1.0f };
};

typedef RGBA ColourClear;

struct DepthStencilClear
{
    float    depth{ 0.0f };
    uint32_t stencil{ 0 };
};

struct RenderArea
{
    int w;
    int h;
};

struct Synchronisation
{
    PipelineStages stages;
    MemoryAccesses accesses;
};

struct AttachmentInfo
{
    Format      format{ Format::R8G8B8A8 };
    MSAASamples samples{ MSAASamples::MSAA_1X };
    LoadOp      load_op{ LoadOp::DONT_CARE };
    StoreOp     store_op{ StoreOp::DONT_CARE };
    LoadOp      stencil_load_op{ LoadOp::DONT_CARE };
    StoreOp     stencil_store_op{ StoreOp::DONT_CARE };
    ImageLayout initial_layout{ ImageLayout::UNDEFINED };
    ImageLayout final_layout{ ImageLayout::UNDEFINED };
};

struct SubpassInfo
{
    PipelineBindPoint bind_point{ PipelineBindPoint::GRAPHICS };
    uint32_t          colour_attachment_infos[rend::constants::max_framebuffer_attachments];
    uint32_t          input_attachment_infos[rend::constants::max_framebuffer_attachments];
    uint32_t          resolve_attachment_infos[rend::constants::max_framebuffer_attachments];
    uint32_t          depth_stencil_attachment{};
    uint32_t          preserve_attachments[rend::constants::max_framebuffer_attachments];
    size_t            colour_attachment_infos_count{ 0 };
    size_t            input_attachment_infos_count{ 0 };
    size_t            resolve_attachment_infos_count{ 0 };
    size_t            preserve_attachments_count{ 0 };
};

struct SubpassDependency
{
    uint32_t                src_subpass{ 0 };
    uint32_t                dst_subpass{ 0 };
    Synchronisation         src_sync{};
    Synchronisation         dst_sync{};
};

struct RenderPassInfo
{
    AttachmentInfo    attachment_infos[rend::constants::max_framebuffer_attachments];
    SubpassInfo       subpasses[rend::constants::max_subpasses];
    SubpassDependency subpass_dependencies[rend::constants::max_subpasses];
    size_t            attachment_infos_count{ 0 };
    size_t            subpasses_count{ 0 };
    size_t            subpass_dependency_count{ 0 };
};

struct FramebufferInfo
{
    uint32_t         width{ 0 };
    uint32_t         height{ 0 };
    uint32_t         depth{ 0 };
    RenderPassHandle render_pass_handle{ NULL_HANDLE };
    Texture2DHandle  depth_buffer_handle{ NULL_HANDLE };
    Texture2DHandle  render_target_handles[rend::constants::max_framebuffer_attachments]{ NULL_HANDLE };
    size_t           render_target_handles_count{ 0 };
};

struct DescriptorSetLayoutBinding
{
    uint32_t       binding{ 0 };
    DescriptorType descriptor_type;
    size_t         descriptor_count{ 0 };
    ShaderStages   shader_stages{ 0 };
};

struct DescriptorSetLayoutInfo
{
    const DescriptorSetLayoutBinding* layout_bindings{ nullptr };
    uint32_t                          layout_bindings_count{ 0 };
};

struct PushConstantRange
{
    ShaderStages shader_stages;
    uint32_t     offset{ 0 };
    size_t       size{ 0 };
};

struct PipelineLayoutInfo
{
    const DescriptorSetLayoutHandle* descriptor_set_layouts{ nullptr };
    uint32_t                         descriptor_set_layout_count{ 0 };
    const PushConstantRange*         push_constant_ranges{ nullptr };
    uint32_t                         push_constant_range_count{ 0 };
};

struct VertexBindingInfo
{
    uint32_t index{ 0 };
    uint32_t stride{ 0 };
};

struct VertexAttributeInfo
{
    uint32_t           shader_location{ 0 };
    uint32_t           offset{ 0 };
    VertexBindingInfo* binding{ nullptr };
    rend::Format       format{ rend::Format::R32G32B32_SFLOAT };
};

struct ViewportInfo
{
    float x{ 0 };
    float y{ 0 };
    float width{ 0.0f };
    float height{ 0.0f };
    float min_depth{ 0.0f };
    float max_depth{ 0.0f };
};

struct RasteriserInfo
{
    rend::PolygonMode polygon_mode{ rend::PolygonMode::FILL };
    rend::CullMode    cull_mode{ rend::CullMode::BACK };
    rend::FrontFace   front_face{ rend::FrontFace::CCW };
    float             depth_bias_clamp{ 0.0f };
    float             depth_bias_constant_factor{ 0.0f };
    float             depth_bias_slope_factor{ 0.0f };
    float             line_width{ 1.0f };
    bool              depth_bias_enabled{ false };
    bool              depth_clamp_enabled{ false };
    bool              discard_enabled{ false };

};

struct MultisamplingInfo
{
    MSAASamples sample_count{ MSAASamples::MSAA_1X };
    float       min_sample_shading{ 0.0f };
    uint32_t    sample_mask{ 1 };
    bool        sample_shading_enabled{ false };
    bool        alpha_to_coverage_enabled{ false };
    bool        alpha_to_one_enabled{ false };
};

struct DepthStencilInfo
{
    bool      depth_test_enabled{ true };
    bool      depth_write_enabled{ true };
    CompareOp compare_op{ CompareOp::LESS };
    bool      depth_bounds_test_enabled{ false };
    bool      stencil_test_enabled{ false };
    StencilOp front_stencil_fail_op{ StencilOp::KEEP };
    StencilOp front_stencil_success_op{ StencilOp::REPLACE };
    StencilOp front_stencil_depth_fail_op{ StencilOp::KEEP };
    CompareOp front_stencil_compare_op{ CompareOp::ALWAYS };
    uint32_t  front_stencil_compare_mask{ 1 };
    uint32_t  front_stencil_write_mask{ 1 };
    uint32_t  front_stencil_reference { 0 };
    StencilOp back_stencil_fail_op{ StencilOp::KEEP };
    StencilOp back_stencil_success_op{ StencilOp::REPLACE };
    StencilOp back_stencil_depth_fail_op{ StencilOp::KEEP };
    CompareOp back_stencil_compare_op{ CompareOp::ALWAYS };
    uint32_t  back_stencil_compare_mask{ 1 };
    uint32_t  back_stencil_write_mask{ 1 };
    uint32_t  back_stencil_reference { 0 };
    float     min_depth_bound{ 0.0f };
    float     max_depth_bound{ 1.0f };
};

struct ColourBlendAttachment
{
    bool             blend_enabled{ false };
    ColourComponents colour_write_mask{ COLOUR_COMPONENT_R | COLOUR_COMPONENT_G | COLOUR_COMPONENT_B | COLOUR_COMPONENT_R };
    BlendFactor      colour_src_factor{ BlendFactor::ONE };
    BlendFactor      colour_dst_factor{ BlendFactor::ZERO };
    BlendOp          colour_blend_op{ BlendOp::ADD };
    BlendFactor      alpha_src_factor{ BlendFactor::SRC_ALPHA };
    BlendFactor      alpha_dst_factor{ BlendFactor::ONE_MINUS_SRC_ALPHA };
    BlendOp          alpha_blend_op{ BlendOp::ADD };
};

struct ColourBlendingInfo
{
    bool                  logic_op_enabled{ false };
    LogicOp               logic_op{ LogicOp::NO_OP }; 
    float                 blend_constants[constants::max_blend_constants];
    ColourBlendAttachment blend_attachments[constants::max_framebuffer_attachments];
    size_t                blend_attachments_count{ 0 };
};

struct PipelineInfo
{
    // Shader State
    ShaderHandle        shaders[(uint32_t)ShaderStage::SHADER_STAGE_COUNT];

    // Vertex Input State
    VertexBindingInfo   vertex_binding_info{};
    VertexAttributeInfo vertex_attribute_infos[constants::max_vertex_attributes];
    size_t              vertex_attribute_info_count{ 0 };

    // Input Assembly State
    Topology topology{ Topology::TRIANGLE_LIST };
    bool     primitive_restart{ false };

    // Tessellation State
    uint32_t patch_control_points{ 0 };

    // Viewport State
    ViewportInfo      viewport_info[constants::max_viewports];
    size_t            viewport_info_count{ 0 };
    ViewportInfo      scissor_info[constants::max_scissors];
    size_t            scissor_info_count{ 0 };

    RasteriserInfo     rasteriser_info{};
    MultisamplingInfo  multisampling_info{};
    DepthStencilInfo   depth_stencil_info{};
    ColourBlendingInfo colour_blending_info{};
    DynamicStates      dynamic_states{ (uint32_t)DynamicState::NONE };

    PipelineLayoutHandle layout_handle{ NULL_HANDLE };
    RenderPassHandle     render_pass_handle{ NULL_HANDLE };
    int32_t subpass{ 0 };
};

struct BufferBufferCopyInfo
{
    size_t size_bytes;
    size_t src_offset;
    size_t dst_offset;
};

struct BufferImageCopyInfo
{
    size_t      buffer_offset{ 0 };
    size_t      buffer_width{ 0 };
    size_t      buffer_height{ 0 };
    size_t      image_offset_x{ 0 };
    size_t      image_offset_y{ 0 };
    size_t      image_offset_z{ 0 };
    size_t      image_width{ 0 };
    size_t      image_height{ 0 };
    size_t      image_depth{ 0 };
    ImageLayout image_layout{ ImageLayout::UNDEFINED };
    size_t      mip_level{ 0 };
    size_t      base_layer{ 0 };
    size_t      layer_count{ 0 };
};

struct ImageMemoryBarrier
{
    MemoryAccesses src_accesses{ MemoryAccess::NO_ACCESS };
    MemoryAccesses dst_accesses{ MemoryAccess::NO_ACCESS } ;
    ImageLayout    old_layout{ ImageLayout::UNDEFINED };
    ImageLayout    new_layout{ ImageLayout::UNDEFINED };
    TextureHandle  image_handle{ NULL_HANDLE };
    uint32_t       mip_level_count{ 0 };
    uint32_t       base_mip_level{ 0 };
    uint32_t       layers_count{ 0 };
    uint32_t       base_layer{ 0 };
};

struct PipelineBarrierInfo
{
    PipelineStages      src_stages{};
    PipelineStages      dst_stages{};
    ImageMemoryBarrier* image_memory_barriers{ nullptr };
    size_t              image_memory_barrier_count{ 0 };
};

struct DescriptorPoolSize
{
    DescriptorType type;
    uint32_t       count;
};

struct DescriptorPoolInfo
{
    DescriptorPoolSize* pool_sizes{ nullptr };
    uint32_t            pool_sizes_count{ 0 };
    uint32_t            max_sets{ 0 };
};

struct DescriptorSetBinding
{
    uint32_t             slot;
    rend::DescriptorType type;
    void*                resource;
};

struct DescriptorSetInfo
{
    DescriptorPoolHandle         pool_handle{ NULL_HANDLE };
    DescriptorSetLayoutHandle    layout_handle{ NULL_HANDLE };
    rend::DescriptorSetBinding*  bindings{ nullptr };
    uint32_t                     bindings_count{ 0 };
};

}

#endif
