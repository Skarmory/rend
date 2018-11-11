#include "swapchain.h"

#include "device_context.h"
#include "physical_device.h"

#include <iostream>
#include <stdexcept>

using namespace rend;

Swapchain::Swapchain(const LogicalDevice* const logical_device, uint32_t desired_images) : _logical_device(logical_device), _image_count(0)
{
    std::cout << "Constructing swap chain" << std::endl;

    if(desired_images == 0)
        throw std::runtime_error("Failed to create swapchain: desired images cannot be 0");

    _create(desired_images);
}

Swapchain::~Swapchain(void)
{
    std::cout << "Destructing swap chain" << std::endl;

    vkDestroySwapchainKHR(_logical_device->get_handle({}), _vk_swapchain, nullptr);
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

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = _logical_device->get_device_context().get_surface({}),
        .minImageCount = _image_count,
        .imageFormat = _surface_format.format,
        .imageColorSpace = _surface_format.colorSpace,
        .imageExtent = surface_caps.maxImageExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = _present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = _vk_swapchain 
    };

    if(vkCreateSwapchainKHR(_logical_device->get_handle({}), &create_info, nullptr, &_vk_swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain");
}

VkSurfaceFormatKHR Swapchain::_find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats)
{
    if(surface_formats.size() == 0)
        throw std::runtime_error("No surface formats found");

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
    if(present_modes.size() == 0)
        throw std::runtime_error("No present modes found");

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
