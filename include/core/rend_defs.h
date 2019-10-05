#ifndef REND_DEFS_H
#define REND_DEFS_H

#define UU(x) ((void)x)

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

enum class TextureFormat
{
    R8G8B8A8
};

}

#endif
