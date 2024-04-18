#ifndef REND_API_VULKAN_SWAPCHAIN_H
#define REND_API_VULKAN_SWAPCHAIN_H

#include "api/vulkan/queue_family.h"
#include "core/rend_defs.h"
#include <vector>
#include <vulkan.h>

namespace rend
{

class Fence;
class GPUTexture;
class LogicalDevice;
class RenderTarget;
class Semaphore;
class VulkanDeviceContext;

class Swapchain
{
public:
    explicit Swapchain(uint32_t desired_images, VulkanDeviceContext& ctx);
    ~Swapchain(void);
    Swapchain(const Swapchain&)            = delete;
    Swapchain(Swapchain&&)                 = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&)      = delete;

    StatusCode recreate(void);

    // Accessors
    VkImage                     get_back_buffer_texture(uint32_t idx);
    const std::vector<VkImage>& get_back_buffer_textures(void);
    uint32_t                    get_current_image_index(void) const;
    VkExtent2D                  get_extent(void) const;
    Format                      get_format(void) const;
    VkSwapchainKHR              vk_handle(void) const;

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
    uint32_t                 _image_count { 0 };
    uint32_t                 _current_image_idx { 0 };
    VkSurfaceFormatKHR       _surface_format {};
    VkPresentModeKHR         _present_mode { VK_PRESENT_MODE_IMMEDIATE_KHR };
    VkSwapchainKHR           _vk_swapchain { VK_NULL_HANDLE };
    VkExtent2D               _vk_extent {};
    std::vector<VkImage>     _vk_swapchain_images;
    VulkanDeviceContext*     _ctx{ nullptr };
};

}

#endif
