#include "api/vulkan/swapchain.h"

#include "core/device_context.h"
#include "core/window.h"
#include "core/window_context.h"

#include "api/vulkan/fence.h"
#include "api/vulkan/physical_device.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_semaphore.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_device_context.h"

#include <cassert>
#include <limits>

using namespace rend;

Swapchain::Swapchain(uint32_t desired_images)
{
    _create(desired_images);
}

Swapchain::~Swapchain(void)
{
    _clean_up_images();

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_swapchain(_vk_swapchain);
    _image_count = 0;
    _current_image_idx = 0;
    _surface_format = {};
    _present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    _vk_swapchain = VK_NULL_HANDLE;
    _vk_extent = {};
    _swapchain_image_handles.clear();
}

StatusCode Swapchain::recreate(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->wait_idle();

    _clean_up_images();
    return _create(_image_count);
}

Format Swapchain::get_format(void) const
{
    return vulkan_helpers::convert_format(_surface_format.format);
}

std::vector<Texture2DHandle> Swapchain::get_back_buffer_handles(void)
{
    return _swapchain_image_handles;
}

Texture2DHandle Swapchain::get_back_buffer_handle(uint32_t idx)
{
    return _swapchain_image_handles[idx];
}

VkExtent2D Swapchain::get_extent(void) const
{
    return _vk_extent;
}

VkSwapchainKHR Swapchain::get_handle(void) const
{
    return _vk_swapchain;
}

uint32_t Swapchain::get_current_image_index(void) const
{
    return _current_image_idx;
}

StatusCode Swapchain::acquire(Semaphore* signal_sem, Fence* acquire_fence)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    VkResult result = ctx.get_device()->acquire_next_image(
        this, std::numeric_limits<uint64_t>::max(), signal_sem, acquire_fence, &_current_image_idx
    );

    if(result != VK_SUCCESS)
    {
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return StatusCode::SWAPCHAIN_OUT_OF_DATE;
        }
        else
        {
            return StatusCode::SWAPCHAIN_ACQUIRE_ERROR;
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode Swapchain::present(QueueType type, const std::vector<Semaphore*>& wait_sems)
{
    std::vector<VkResult> results(1);

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    if(ctx.get_device()->queue_present(type, wait_sems, { this }, { _current_image_idx }, results))
    {
        return StatusCode::FAILURE;
    }

    if(results[0] != VK_SUCCESS)
    {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode Swapchain::_create(uint32_t desired_images)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    const PhysicalDevice& physical_device = ctx.get_device()->get_physical_device();

    VkSurfaceCapabilitiesKHR surface_caps = physical_device.get_surface_capabilities();

    if(physical_device.get_surface_formats().empty())
    {
        return StatusCode::SWAPCHAIN_NO_SURFACE_FORMATS_FOUND;
    }

    if(physical_device.get_surface_present_modes().empty())
    {
        return StatusCode::SWAPCHAIN_NO_SURFACE_PRESENT_MODES_FOUND;
    }

    _surface_format = _find_surface_format(physical_device.get_surface_formats());
    _present_mode   = _find_present_mode(physical_device.get_surface_present_modes());
    _image_count    = _find_image_count(desired_images, surface_caps);

    _vk_extent = surface_caps.currentExtent;

    VkSwapchainKHR old_swapchain = _vk_swapchain;

    VkSwapchainCreateInfoKHR create_info = vulkan_helpers::gen_swapchain_create_info();
    create_info.surface               = WindowContext::instance().window()->get_vk_surface();
    create_info.minImageCount         = _image_count;
    create_info.imageFormat           = _surface_format.format;
    create_info.imageColorSpace       = _surface_format.colorSpace;
    create_info.imageExtent           = surface_caps.currentExtent;
    create_info.imageArrayLayers      = 1;
    create_info.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    create_info.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode           = _present_mode;
    create_info.oldSwapchain          = old_swapchain;

    _vk_swapchain = ctx.get_device()->create_swapchain(create_info);
    if(_vk_swapchain == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    if(old_swapchain != VK_NULL_HANDLE)
    {
        ctx.get_device()->destroy_swapchain(old_swapchain);
    }

    return _get_images();
}

void Swapchain::_clean_up_images(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    for(auto handle : _swapchain_image_handles)
    {
        ctx.unregister_swapchain_image(handle);
    }

    _swapchain_image_handles.clear();
}

StatusCode Swapchain::_get_images(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    std::vector<VkImage> tmp_swapchain_images;
    ctx.get_device()->get_swapchain_images(this, tmp_swapchain_images);

    for(size_t idx = 0; idx < tmp_swapchain_images.size(); idx++)
    {
        _swapchain_image_handles.push_back(ctx.register_swapchain_image(tmp_swapchain_images[idx], _surface_format.format));
    }

    return StatusCode::SUCCESS;
}

VkSurfaceFormatKHR Swapchain::_find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats)
{
    if(surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
    {
        return VkSurfaceFormatKHR{ .format=VK_FORMAT_B8G8R8A8_UNORM, .colorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }

    for(VkSurfaceFormatKHR surface_format : surface_formats)
    {
        if(surface_format.format == VK_FORMAT_B8G8R8A8_UNORM && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return surface_format;
        }
    }

    return surface_formats[0];
}

VkPresentModeKHR Swapchain::_find_present_mode(const std::vector<VkPresentModeKHR>& present_modes)
{
    VkPresentModeKHR chosen = VK_PRESENT_MODE_FIFO_KHR;
    for(VkPresentModeKHR present_mode : present_modes)
    {
        if(present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return present_mode;
        }

        if(present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            chosen = present_mode; 
        }
    }

    return chosen;
}

uint32_t Swapchain::_find_image_count(uint32_t desired_images, const VkSurfaceCapabilitiesKHR& surface_caps)
{
    _image_count = desired_images;

    if(surface_caps.maxImageCount > 0 && desired_images > surface_caps.maxImageCount)
    {
        _image_count = surface_caps.maxImageCount;
    }
    else if(surface_caps.minImageCount > 0 && desired_images < surface_caps.minImageCount + 1)
    {
        _image_count = surface_caps.minImageCount + 1;
    }

    return _image_count;
}
