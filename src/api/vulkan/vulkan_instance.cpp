#include "api/vulkan/vulkan_instance.h"

#include <cassert>

using namespace rend;

VulkanInstance& VulkanInstance::instance(void)
{
    static VulkanInstance s_instance;

    return s_instance;
}

StatusCode VulkanInstance::create(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count)
{
    assert(_vk_instance == VK_NULL_HANDLE && "Attempt to create a VulkanInstance that has already been created.");

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

    if (vkCreateInstance(&instance_create_info, nullptr, &_vk_instance) != VK_SUCCESS)
    {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

void VulkanInstance::destroy(void)
{
    vkDestroyInstance(_vk_instance, nullptr);
    _vk_instance = VK_NULL_HANDLE;
}

void VulkanInstance::enumerate_physical_devices(std::vector<VkPhysicalDevice>& devices)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(_vk_instance, &device_count, nullptr);

    devices.resize(device_count);
    vkEnumeratePhysicalDevices(_vk_instance, &device_count, devices.data());
}

void VulkanInstance::destroy_surface(VkSurfaceKHR surface)
{
    vkDestroySurfaceKHR(_vk_instance, surface, nullptr);
}

VkInstance VulkanInstance::get_handle(void) const
{
    return _vk_instance;
}
