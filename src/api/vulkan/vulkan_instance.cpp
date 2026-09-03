#include "rend/api/vulkan/vulkan_instance.h"

#include "rend/api/vulkan/extension_funcs.h"

#include "rend/core/window.h"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace rend;

VulkanInstance::VulkanInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
{
    VkApplicationInfo app_info =
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = 1,
        .pEngineName = "Rend",
        .engineVersion = 1,
        .apiVersion = VK_API_VERSION_1_3
    };

    VkInstanceCreateInfo instance_create_info =
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    if (auto code = vkCreateInstance(&instance_create_info, nullptr, &_vk_instance); code != VK_SUCCESS)
    {
        std::stringstream error_string;
        error_string << "Failed to create Vulkan instance: " << code;
        throw std::runtime_error(error_string.str());
    }

    _ext_funcs.pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(_vk_instance, "vkCreateDebugUtilsMessengerEXT");
    _ext_funcs.pfnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(_vk_instance, "vkDestroyDebugUtilsMessengerEXT");
}

VulkanInstance::~VulkanInstance(void)
{
    vkDestroySurfaceKHR(_vk_instance, _vk_surface, nullptr);
    vkDestroyInstance(_vk_instance, nullptr);
}

void VulkanInstance::enumerate_physical_devices(std::vector<VkPhysicalDevice>& devices) const
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(_vk_instance, &device_count, nullptr);

    devices.resize(device_count);
    vkEnumeratePhysicalDevices(_vk_instance, &device_count, devices.data());
}

void VulkanInstance::create_surface(const Window& window)
{
    glfwCreateWindowSurface(_vk_instance, (GLFWwindow*)window.get_handle(), nullptr, &_vk_surface);
}

VkDebugUtilsMessengerEXT VulkanInstance::create_debug_utils_messenger(const VkDebugUtilsMessengerCreateInfoEXT& create_info) const
{
    VkDebugUtilsMessengerEXT messenger;
    _ext_funcs.pfnCreateDebugUtilsMessengerEXT(_vk_instance, &create_info, NULL, &messenger);
    return messenger;
}

void VulkanInstance::destroy_debug_utils_messenger(VkDebugUtilsMessengerEXT messenger) const
{
    _ext_funcs.pfnDestroyDebugUtilsMessengerEXT(_vk_instance, messenger, NULL);
}

VkInstance VulkanInstance::get_handle(void) const
{
    return _vk_instance;
}

VkSurfaceKHR VulkanInstance::surface(void) const
{
    return _vk_surface;
}
