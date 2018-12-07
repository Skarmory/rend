#include "swapchain.h"

#include "device_context.h"
#include "fence.h"
#include "physical_device.h"
#include "logical_device.h"
#include "semaphore.h"
#include "utils.h"

#include <algorithm>
#include <iostream>
#include <limits>

using namespace rend;

Swapchain::Swapchain(const LogicalDevice* const logical_device, uint32_t desired_images) : _logical_device(logical_device), _image_count(0), _current_image_idx(0), _vk_swapchain(VK_NULL_HANDLE)
{
    std::cout << "Constructing swap chain" << std::endl;

    DEATH_CHECK(desired_images == 0, "Failed to create swapchain: desired images cannot be 0");

    _create(desired_images);

    _get_images();
}

Swapchain::~Swapchain(void)
{
    std::cout << "Destructing swap chain" << std::endl;

    _destroy();
    vkDestroySwapchainKHR(_logical_device->get_handle(), _vk_swapchain, nullptr);
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

void Swapchain::recreate(void)
{
    vkDeviceWaitIdle(_logical_device->get_handle());
    _destroy();
    _create(_image_count);
    _get_images();
}

uint32_t Swapchain::acquire(Semaphore* signal_sem, Fence* acquire_fence)
{
    VkResult result = vkAcquireNextImageKHR(
        _logical_device->get_handle(),
        _vk_swapchain,
        std::numeric_limits<uint64_t>::max(),
        signal_sem ? signal_sem->get_handle() : VK_NULL_HANDLE,
        acquire_fence ? acquire_fence->get_handle() : VK_NULL_HANDLE,
        &_current_image_idx
    );

    if(result != VK_SUCCESS)
    {
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return std::numeric_limits<uint32_t>::max();
        }
        else
        {
            DEATH_CHECK(false, "Failed to acquire next swapchain image");
        }
    }

    return _current_image_idx;
}

void Swapchain::present(QueueType type, const std::vector<Semaphore*>& wait_sems)
{
    std::vector<VkSemaphore> vk_sems;
    vk_sems.reserve(wait_sems.size());

    std::for_each(wait_sems.begin(), wait_sems.end(), [&vk_sems](Semaphore* s){ vk_sems.push_back(s->get_handle()); });

    VkPresentInfoKHR present_info =
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(wait_sems.size()),
        .pWaitSemaphores = vk_sems.data(),
        .swapchainCount = 1,
        .pSwapchains = &_vk_swapchain,
        .pImageIndices = &_current_image_idx,
        .pResults = nullptr
    };

    VULKAN_DEATH_CHECK(vkQueuePresentKHR(_logical_device->get_queue(type), &present_info), "Failed to present queue");
}

void Swapchain::_create(uint32_t desired_images)
{
    const PhysicalDevice& physical_device = _logical_device->get_physical_device();

    VkSurfaceCapabilitiesKHR surface_caps = physical_device.get_surface_capabilities();

    _surface_format = _find_surface_format(physical_device.get_surface_formats());
    std::cout << "Chosen format: " << _surface_format.format << " // " << _surface_format.colorSpace << std::endl;

    _present_mode = _find_present_mode(physical_device.get_surface_present_modes());
    std::cout << "Chosen present mode: " << _present_mode << std::endl;

    _image_count = _find_image_count(desired_images, surface_caps);
    std::cout << "Chosen image count: " << _image_count << std::endl;

    _vk_extent = surface_caps.currentExtent;

    VkSwapchainKHR old_swapchain = _vk_swapchain;

    VkSwapchainCreateInfoKHR create_info = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0,
        .surface               = _logical_device->get_device_context().get_surface(),
        .minImageCount         = _image_count,
        .imageFormat           = _surface_format.format,
        .imageColorSpace       = _surface_format.colorSpace,
        .imageExtent           = surface_caps.currentExtent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = _present_mode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = old_swapchain
    };

    VULKAN_DEATH_CHECK(vkCreateSwapchainKHR(_logical_device->get_handle(), &create_info, nullptr, &_vk_swapchain), "Failed to create swapchain");

    if(old_swapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(_logical_device->get_handle(), old_swapchain, nullptr);
}

void Swapchain::_destroy(void)
{
    for(size_t idx = 0; idx < _image_count; idx++)
        vkDestroyImageView(_logical_device->get_handle(), _vk_image_views[idx], nullptr);
}

void Swapchain::_get_images(void)
{
    vkGetSwapchainImagesKHR(_logical_device->get_handle(), _vk_swapchain, &_image_count, nullptr);

    _vk_images.resize(_image_count);

    vkGetSwapchainImagesKHR(_logical_device->get_handle(), _vk_swapchain, &_image_count, _vk_images.data());

    _vk_image_views.resize(_image_count);

    std::cout << "Image count: " << _image_count << std::endl;

    VkImageViewCreateInfo image_view_create_info = {};
    image_view_create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.format                          = _surface_format.format;
    image_view_create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel   = 0;
    image_view_create_info.subresourceRange.levelCount     = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount     = 1;

    for(size_t idx = 0; idx < _vk_images.size(); idx++)
    {
        image_view_create_info.image = _vk_images[idx];
        VULKAN_DEATH_CHECK(vkCreateImageView(_logical_device->get_handle(), &image_view_create_info, nullptr, &_vk_image_views[idx]), "Failed to create swapchain image image view");
    }
}

VkSurfaceFormatKHR Swapchain::_find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats)
{
    DEATH_CHECK(surface_formats.size() == 0, "No surface formats found");

    if(surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
        return VkSurfaceFormatKHR{ .format=VK_FORMAT_B8G8R8A8_UNORM, .colorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    for(VkSurfaceFormatKHR surface_format : surface_formats)
    {
        if(surface_format.format == VK_FORMAT_B8G8R8A8_UNORM && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return surface_format;
    }

    std::cout << "No good surface formats found. Defaulting to: " << surface_formats[0].format << " // " << surface_formats[0].colorSpace << std::endl;
    return surface_formats[0];
}

VkPresentModeKHR Swapchain::_find_present_mode(const std::vector<VkPresentModeKHR>& present_modes)
{
    DEATH_CHECK(present_modes.size() == 0, "No present modes found");

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
        std::cout << "Too many images requested. Device only supports " << surface_caps.maxImageCount
                  << ". Setting image count to that." << std::endl;

        _image_count = surface_caps.maxImageCount;
    }
    else if(surface_caps.minImageCount > 0 && desired_images < surface_caps.minImageCount + 1)
    {
        std::cout << "Too few images requested. Device requires at least " << surface_caps.minImageCount
                  << ". Setting image count to that." << std::endl;

        _image_count = surface_caps.minImageCount + 1;
    }

    return _image_count;
}
