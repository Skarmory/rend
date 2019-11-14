#include "swapchain.h"

#include "device_context.h"
#include "fence.h"
#include "physical_device.h"
#include "logical_device.h"
#include "render_target.h"
#include "semaphore.h"
#include "window.h"
#include "vulkan_helper_funcs.h"

#include <cassert>
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

std::vector<RenderTarget*>& Swapchain::get_render_targets(void)
{
    return _render_targets;
}

RenderTarget& Swapchain::get_render_target(uint32_t idx)
{
    assert(idx < _render_targets.size());
    return *_render_targets[idx];
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

    VkSwapchainCreateInfoKHR create_info = vulkan_helpers::gen_swapchain_create_info();
    create_info.surface               = _context.get_window()->get_handle();
    create_info.minImageCount         = _image_count;
    create_info.imageFormat           = _surface_format.format;
    create_info.imageColorSpace       = _surface_format.colorSpace;
    create_info.imageExtent           = surface_caps.currentExtent;
    create_info.imageArrayLayers      = 1;
    create_info.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    create_info.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode           = _present_mode;
    create_info.oldSwapchain          = old_swapchain;

    _vk_swapchain = _context.get_device()->create_swapchain(create_info);
    if(_vk_swapchain == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

    if(old_swapchain != VK_NULL_HANDLE)
        _context.get_device()->destroy_swapchain(old_swapchain);

    return _get_images();
}

void Swapchain::_destroy_image_views(void)
{
    for(size_t idx = 0; idx < _render_targets.size(); idx++)
        _context.get_device()->destroy_image_view(_render_targets[idx]->_vk_image_view);
}

StatusCode Swapchain::_get_images(void)
{
    std::vector<VkImage> images;
    _context.get_device()->get_swapchain_images(this, images);
    _render_targets.resize(images.size());

    for(size_t idx = 0; idx < images.size(); idx++)
    {
        if(!_render_targets[idx])
            _render_targets[idx] = new RenderTarget(_context);

        VkImageViewCreateInfo image_view_info = vulkan_helpers::gen_image_view_create_info();
        image_view_info.image                           = images[idx];
        image_view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format                          = _surface_format.format;
        image_view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseMipLevel   = 0;
        image_view_info.subresourceRange.levelCount     = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount     = 1;

        VkImageView view = _context.get_device()->create_image_view(image_view_info);
        if(view == VK_NULL_HANDLE)
            return StatusCode::FAILURE;

        _render_targets[idx]->_vk_image      = images[idx];
        _render_targets[idx]->_vk_image_view = view;
        _render_targets[idx]->_vk_type       = VK_IMAGE_TYPE_2D;
        _render_targets[idx]->_vk_format     = _surface_format.format;
        _render_targets[idx]->_mip_levels    = 1;
        _render_targets[idx]->_array_layers  = 1;
        _render_targets[idx]->_vk_samples    = VK_SAMPLE_COUNT_1_BIT;
        _render_targets[idx]->_vk_tiling     = VK_IMAGE_TILING_OPTIMAL;
        _render_targets[idx]->_vk_usage      = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        _render_targets[idx]->_vk_layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
