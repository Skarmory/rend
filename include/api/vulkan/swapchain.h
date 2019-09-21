#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <limits>
#include <vulkan.h>
#include <vector>

#include "queue_family.h"
#include "rend_defs.h"

namespace rend
{

class DeviceContext;
class Fence;
class LogicalDevice;
class Semaphore;

class Swapchain
{
public:
    Swapchain(DeviceContext* context);
    ~Swapchain(void);
    Swapchain(const Swapchain&)            = delete;
    Swapchain(Swapchain&&)                 = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&)      = delete;

    VkFormat                        get_format(void) const;
    const std::vector<VkImage>&     get_images(void) const;
    const std::vector<VkImageView>& get_image_views(void) const;
    VkExtent2D                      get_extent(void) const;
    VkSwapchainKHR                  get_handle(void) const;

    void     create_swapchain(uint32_t desired_images);
    void     recreate(void);
    uint32_t acquire(Semaphore* signal_sem, Fence* acquire_fence);
    void     present(QueueType type, const std::vector<Semaphore*>& wait_sems);

private:
    void               _destroy(void);
    void               _get_images(void);
    VkSurfaceFormatKHR _find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats);
    VkPresentModeKHR   _find_present_mode(const std::vector<VkPresentModeKHR>& present_modes);
    uint32_t           _find_image_count(uint32_t desired_images, const VkSurfaceCapabilitiesKHR& surface_caps);

private:
    DeviceContext*     _context;
    uint32_t           _image_count;
    uint32_t           _current_image_idx;
    VkSurfaceFormatKHR _surface_format;
    VkPresentModeKHR   _present_mode;
    VkSwapchainKHR     _vk_swapchain;
    VkExtent2D         _vk_extent;

    std::vector<VkImage>       _vk_images;
    std::vector<VkImageView>   _vk_image_views;
};

}

#endif
