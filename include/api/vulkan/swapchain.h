#ifndef REND_API_VULKAN_SWAPCHAIN_H
#define REND_API_VULKAN_SWAPCHAIN_H

#include "api/vulkan/queue_family.h"
#include "api/vulkan/swapchain_acquire.h"
#include "core/containers/data_pool.h"
#include "core/containers/ring_buffer.h"
#include "core/rend_defs.h"
#include "core/texture_info.h"

#include <string>
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
class VulkanTexture;

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
    VkExtent2D     get_extent(void) const;
    Format         get_format(void) const;
    VkSwapchainKHR vk_handle(void) const;
    const VulkanTexture& get_backbuffer(uint32_t backbuffer_idx) const;
    TextureInfo get_backbuffer_texture_info(void) const;

    // Mutators
    StatusCode acquire(SwapchainAcquire** out_acquire);
    StatusCode present(const SwapchainAcquire& acquisition, QueueType type /*, const std::vector<Semaphore*>& wait_sems */);

private:
    StatusCode         _create(void);
    void               _clean_up_images(void);
    StatusCode         _get_images(void);
    VkSurfaceFormatKHR _find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats);
    VkPresentModeKHR   _find_present_mode(const std::vector<VkPresentModeKHR>& present_modes);
    uint32_t           _find_image_count(uint32_t desired_images, const VkSurfaceCapabilitiesKHR& surface_caps);
    TextureHandle      _register_swapchain_image(const std::string& name, VkImage image);
    void               _unregister_swapchain_image(TextureHandle texture_handle);

private:
    uint32_t             _desired_image_count { 0 };
    uint32_t             _image_idx { 0 };
    uint32_t             _create_count{ 0 };
    VkSurfaceFormatKHR   _surface_format {};
    VkPresentModeKHR     _present_mode { VK_PRESENT_MODE_IMMEDIATE_KHR };
    VkSwapchainKHR       _vk_swapchain { VK_NULL_HANDLE };
    VkExtent2D           _vk_extent {};
    rend::TextureInfo    _backbuffer_texture_info{};
    VulkanDeviceContext* _ctx{ nullptr };

    struct
    {
        std::vector<VkImage>               vk_swapchain_images;
        std::vector<TextureHandle>         backbuffer_handles;
        rend::RingBuffer<SwapchainAcquire> swapchain_acquires;
        rend::DataPool<VulkanTexture, 8>   backbuffer_textures;
    } _backbuffer_resources;
};

}

#endif
