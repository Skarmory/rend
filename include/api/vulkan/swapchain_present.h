#ifndef REND_API_VULKAN_SWAPCHAIN_PRESENT_H
#define REND_API_VULKAN_SWAPCHAIN_PRESENT_H

#include "core/rend_defs.h"

namespace rend
{

class Semaphore;

struct SwapchainPresent
{
    uint32_t image_idx{ 0 };
    Semaphore* semaphore{ nullptr };
};

}

#endif

