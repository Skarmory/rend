#include "swapchain.h"

#include "device_context.h"
#include "fence.h"
#include "physical_device.h"
#include "logical_device.h"
#include "rend_defs.h"
#include "semaphore.h"
#include "window.h"

#include <limits>

using namespace rend;

Swapchain::Swapchain(DeviceContext& context)
    : _context(context),
      _image_count(0),
      _current_image_idx(0),
      _surface_format({}),
      _present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR),
      _vk_swapchain(VK_NULL_HANDLE),
      _vk_extent({})
{
}

Swapchain::~Swapchain(void)
{
    _destroy_image_views();
    _context.get_device()->destroy_swapchain(_vk_swapchain);
}

VkFormat Swapchain::get_format(void) const
{
    return _surface_format.format;
}

const std::vector<VkImage>& Swapchain::get_images(void) const
{
    return _vk_images;
}

const std::vector<VkImageView>& Swapchain::get_image_views(void) const
{
    return _vk_image_views;
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

StatusCode Swapchain::create_swapchain(uint32_t desired_images)
{
    if(_vk_swapchain != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    return _create_swapchain(desired_images);
}

StatusCode Swapchain::recreate(void)
{
    _context.get_device()->wait_idle();
    _destroy_image_views();
    return _create_swapchain(_image_count);
}

StatusCode Swapchain::acquire(Semaphore* signal_sem, Fence* acquire_fence)
{
    VkResult result = _context.get_device()->acquire_next_image(
        this, std::numeric_limits<uint64_t>::max(), signal_sem, acquire_fence, &_current_image_idx
    );

    if(result != VK_SUCCESS)
    {
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
            return StatusCode::SWAPCHAIN_OUT_OF_DATE;
        else
            return StatusCode::SWAPCHAIN_ACQUIRE_ERROR;
    }

    return StatusCode::SUCCESS;
}

StatusCode Swapchain::present(QueueType type, const std::vector<Semaphore*>& wait_sems)
{
    std::vector<VkResult> results(1);
    if(_context.get_device()->queue_present(type, wait_sems, { this }, { _current_image_idx }, results))
        return StatusCode::FAILURE;

    if(results[0] != VK_SUCCESS)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

StatusCode Swapchain::_create_swapchain(uint32_t desired_images)
{
    const PhysicalDevice& physical_device = _context.get_device()->get_physical_device();

    VkSurfaceCapabilitiesKHR surface_caps = physical_device.get_surface_capabilities();

    if(physical_device.get_surface_formats().empty())
        return StatusCode::SWAPCHAIN_NO_SURFACE_FORMATS_FOUND;

    if(physical_device.get_surface_present_modes().empty())
        return StatusCode::SWAPCHAIN_NO_SURFACE_PRESENT_MODES_FOUND;

    _surface_format = _find_surface_format(physical_device.get_surface_formats());
    _present_mode   = _find_present_mode(physical_device.get_surface_present_modes());
    _image_count    = _find_image_count(desired_images, surface_caps);

    _vk_extent = surface_caps.currentExtent;

    VkSwapchainKHR old_swapchain = _vk_swapchain;

    _vk_swapchain = _context.get_device()->create_swapchain(
        _context.get_window()->get_handle(), _image_count, _surface_format.format,
        _surface_format.colorSpace, surface_caps.currentExtent, 1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0,
        nullptr, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        _present_mode, VK_TRUE, old_swapchain
    );

    if(_vk_swapchain == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

    if(old_swapchain != VK_NULL_HANDLE)
        _context.get_device()->destroy_swapchain(old_swapchain);

    return _get_images();
}

void Swapchain::_destroy_image_views(void)
{
    for(size_t idx = 0; idx < _image_count; idx++)
        _context.get_device()->destroy_image_view(_vk_image_views[idx]);
}

StatusCode Swapchain::_get_images(void)
{
    _context.get_device()->get_swapchain_images(this, _vk_images);
    _image_count = _vk_images.size();
    _vk_image_views.resize(_image_count);

    for(size_t idx = 0; idx < _vk_images.size(); idx++)
    {
        _vk_image_views[idx] = _context.get_device()->create_image_view(
            _vk_images[idx], VK_IMAGE_VIEW_TYPE_2D, _surface_format.format,
            VkComponentMapping{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
            VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
        );

        if(_vk_image_views[idx] == VK_NULL_HANDLE)
            return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

VkSurfaceFormatKHR Swapchain::_find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats)
{
    if(surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
        return VkSurfaceFormatKHR{ .format=VK_FORMAT_B8G8R8A8_UNORM, .colorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    for(VkSurfaceFormatKHR surface_format : surface_formats)
    {
        if(surface_format.format == VK_FORMAT_B8G8R8A8_UNORM && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return surface_format;
    }

    return surface_formats[0];
}

VkPresentModeKHR Swapchain::_find_present_mode(const std::vector<VkPresentModeKHR>& present_modes)
{
    VkPresentModeKHR chosen = VK_PRESENT_MODE_FIFO_KHR;
    for(VkPresentModeKHR present_mode : present_modes)
    {
        if(present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return present_mode;

        if(present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            chosen = present_mode; 
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
