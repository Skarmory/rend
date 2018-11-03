#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan.h>
#include "logical_device.h"

namespace rend
{
class LogicalDevice;

class Swapchain
{
public:
    Swapchain(const LogicalDevice* const logical_device, uint32_t desired_images);
    ~Swapchain(void);

    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&)      = delete;

    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&)      = delete;

private:
    void _create(uint32_t desired_images);
    VkSurfaceFormatKHR _find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats);
    VkPresentModeKHR   _find_present_mode(const std::vector<VkPresentModeKHR>& present_modes);
    uint32_t           _find_image_count(uint32_t desired_images, const VkSurfaceCapabilitiesKHR& surface_caps);

private:
    const LogicalDevice* const _logical_device; 
    uint32_t _image_count;
    VkSurfaceFormatKHR _surface_format;
    VkPresentModeKHR _present_mode;
    VkSwapchainKHR _vk_swapchain;
};

}

#endif
