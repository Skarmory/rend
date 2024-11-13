#ifndef REND_API_VULKAN_SWAPCHAIN_ACQUIRE_H
#define REND_API_VULKAN_SWAPCHAIN_ACQUIRE_H

#include "core/rend_defs.h"

namespace rend
{

class Fence;
class Semaphore;

struct SwapchainAcquire
{
    uint32_t     image_idx{ 0 };
    Semaphore*   acquire_semaphore{ nullptr };
    Semaphore*   present_semaphore{ nullptr };
    Fence*       acquire_fence{ nullptr };
    //Fence*       submit_fence{ nullptr };
    //BufferHandle backbuffer_handle{ NULL_HANDLE };
};

}

#endif
