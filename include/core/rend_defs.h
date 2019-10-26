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

struct Synchronisation
{
    PipelineStages stages;
    MemoryAccesses accesses;
};

}

#endif
