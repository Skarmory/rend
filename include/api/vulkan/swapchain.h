#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan.h>
#include <vector>

#include "core/rend_defs.h"

#include "api/vulkan/queue_family.h"

namespace rend
{

class Fence;
class LogicalDevice;
class RenderTarget;
class Semaphore;

class Swapchain
{
public:
    Swapchain(void)                        = default;
    ~Swapchain(void)                       = default;
    Swapchain(const Swapchain&)            = delete;
    Swapchain(Swapchain&&)                 = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&)      = delete;

    StatusCode create(uint32_t desired_images);
    void       destroy(void);
    StatusCode recreate(void);

    // Accessors
    Format                       get_format(void) const;
    std::vector<Texture2DHandle> get_back_buffer_handles(void);
    Texture2DHandle              get_back_buffer_handle(uint32_t idx);
    VkExtent2D                   get_extent(void) const;
    VkSwapchainKHR               get_handle(void) const;
    uint32_t                     get_current_image_index(void) const;

    // Mutators
    StatusCode acquire(Semaphore* signal_sem, Fence* acquire_fence);
    StatusCode present(QueueType type, const std::vector<Semaphore*>& wait_sems);

private:
    StatusCode         _create(uint32_t desired_images);
    void               _clean_up_images(void);
    StatusCode         _get_images(void);
    VkSurfaceFormatKHR _find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats);
    VkPresentModeKHR   _find_present_mode(const std::vector<VkPresentModeKHR>& present_modes);
    uint32_t           _find_image_count(uint32_t desired_images, const VkSurfaceCapabilitiesKHR& surface_caps);

private:
    uint32_t           _image_count { 0 };
    uint32_t           _current_image_idx { 0 };
    VkSurfaceFormatKHR _surface_format {};
    VkPresentModeKHR   _present_mode { VK_PRESENT_MODE_IMMEDIATE_KHR };
    VkSwapchainKHR     _vk_swapchain { VK_NULL_HANDLE };
    VkExtent2D         _vk_extent {};
    std::vector<Texture2DHandle> _swapchain_image_handles;
};

}

#endif
