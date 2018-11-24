#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan.h>
#include <vector>

#include "queue_family.h"

namespace rend
{

class LogicalDevice;

class Swapchain
{
    friend class LogicalDevice;

public:

    Swapchain(const Swapchain&)            = delete;
    Swapchain(Swapchain&&)                 = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&)      = delete;

    VkFormat                        get_format(void) const;
    const std::vector<VkImage>&     get_images(void) const;
    const std::vector<VkImageView>& get_image_views(void) const;
    VkExtent2D                      get_extent(void) const;
    VkSwapchainKHR                  get_handle(void) const;

    uint32_t acquire(VkSemaphore acquire_semaphore, VkFence acquire_fence);
    void     present(QueueType type, const std::vector<VkSemaphore>& wait_sems);

private:

    Swapchain(const LogicalDevice* const logical_device, uint32_t desired_images);
    ~Swapchain(void);

    void               _create(uint32_t desired_images);
    void               _get_images(void);
    VkSurfaceFormatKHR _find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats);
    VkPresentModeKHR   _find_present_mode(const std::vector<VkPresentModeKHR>& present_modes);
    uint32_t           _find_image_count(uint32_t desired_images, const VkSurfaceCapabilitiesKHR& surface_caps);

private:
    const LogicalDevice* const _logical_device; 
    uint32_t                   _image_count;
    uint32_t                   _current_image_idx;
    VkSurfaceFormatKHR         _surface_format;
    VkPresentModeKHR           _present_mode;
    VkSwapchainKHR             _vk_swapchain;
    VkExtent2D                 _vk_extent;

    std::vector<VkImage>       _vk_images;
    std::vector<VkImageView>   _vk_image_views;
};

}

#endif
