#include "api/vulkan/vulkan_instance.h"

#include "core/window.h"
#include "core/rend_service.h"

#include <GLFW/glfw3.h>

#include <cassert>
#include <iostream>

#include <string>
#include <sstream>

using namespace rend;

VulkanInstance::VulkanInstance(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count)
{
    VkApplicationInfo app_info =
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = 1,
        .pEngineName = "Rend",
        .engineVersion = 1,
        .apiVersion = VK_API_VERSION_1_1
    };

    VkInstanceCreateInfo instance_create_info =
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions
    };

    if (auto code = vkCreateInstance(&instance_create_info, nullptr, &_vk_instance); code != VK_SUCCESS)
    {
        std::stringstream error_string;
        error_string << "Failed to create Vulkan instance: " << code;
        throw std::runtime_error(error_string.str());
    }
}

VulkanInstance::~VulkanInstance(void)
{
    vkDestroySurfaceKHR(_vk_instance, _vk_surface, nullptr);
    vkDestroyInstance(_vk_instance, nullptr);
}

void VulkanInstance::enumerate_physical_devices(std::vector<VkPhysicalDevice>& devices)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(_vk_instance, &device_count, nullptr);

    devices.resize(device_count);
    vkEnumeratePhysicalDevices(_vk_instance, &device_count, devices.data());
}

void VulkanInstance::create_surface(const Window& window)
{
    glfwCreateWindowSurface(_vk_instance, window.get_handle(), nullptr, &_vk_surface);
}

VkInstance VulkanInstance::get_handle(void) const
{
    return _vk_instance;
}

VkSurfaceKHR VulkanInstance::surface(void) const
{
    return _vk_surface;
}
