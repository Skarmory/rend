#ifndef REND_DEFS_H
#define REND_DEFS_H

#include <cstdint>

#define UU(x) ((void)x)
#define BIT(x) (static_cast<uint32_t>(1 << x))

namespace rend
{

enum class StatusCode
{
    // Generic
    SUCCESS,
    FAILURE,
    ALREADY_CREATED,

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
    MEMORY_BIND_IMAGE_FAILURE
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

enum class ShaderType
{
    VERTEX,
    FRAGMENT
};

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
    PRESENT
};

enum PipelineStage
{
    NO_STAGE                = 0,
    TOP_OF_PIPE             = BIT(0),
    DRAW_INDIRECT           = BIT(1),
    VERTEX_INPUT            = BIT(2),
    VERTEX_SHADER           = BIT(3),
    TESSELLATION_CONTROL    = BIT(4),
    TESSELLATION_EVALUATION = BIT(5),
    GEOMETRY_SHADER         = BIT(6),
    TRANSFORM_FEEDBACK      = BIT(7),
    EARLY_FRAGMENT_TEST     = BIT(8),
    FRAGMENT_SHADER         = BIT(9),
    LATE_FRAGMENT_TEST      = BIT(10),
    COLOUR_OUTPUT           = BIT(11),
    BOTTOM_OF_PIPE          = BIT(12)
};

typedef uint32_t PipelineStages;

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

enum class DynamicState
{
    VIEWPORT,
    SCISSOR,
    LINE_WIDTH,
    DEPTH_BIAS,
    BLEND_CONSTANTS,
    DEPTH_BOUNDS,
    STENCIL_COMPARE_MASK,
    STENCIL_WRITE_MASK,
    STENCIL_REFERENCE
};

struct Synchronisation
{
    PipelineStages stages;
    MemoryAccesses accesses;
};

}

#endif
