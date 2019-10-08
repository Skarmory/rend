#ifdef USE_VULKAN
#ifndef REND_VULKAN_WINDOW_H
#define REND_VULKAN_WINDOW_H

#include "window_base.h"

#include <vulkan.h>

namespace rend
{

class VulkanInstance;

class VulkanWindow : public WindowBase
{
public:
    VulkanWindow(VulkanInstance& instance);
    ~VulkanWindow(void);

    VulkanWindow(const VulkanWindow&)            = delete;
    VulkanWindow(VulkanWindow&&)                 = delete;
    VulkanWindow& operator=(const VulkanWindow&) = delete;
    VulkanWindow& operator=(VulkanWindow&&)      = delete;

    VkSurfaceKHR get_handle(void) const;

protected:
    VulkanInstance& _instance;
    VkSurfaceKHR   _vk_surface;
};

}

#endif
#endif
