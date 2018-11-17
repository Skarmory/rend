#include "device_context.h"

#include "physical_device.h"
#include "window.h"
#include "utils.h"

#include <GLFW/glfw3.h>
#include <iostream>

using namespace rend;

DeviceContext::DeviceContext(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count, Window* window) : _window(window)
{
    std::cout << "Constructing device context" << std::endl;

    // Step 1: Create Vulkan instance
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = 1,
        .pEngineName = "Rend",
        .engineVersion = 1,
        .apiVersion = VK_API_VERSION_1_1
    };

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions 
    };

    /*
    if( vkCreateInstance(&instance_create_info, nullptr, &_vk_instance) != VK_SUCCESS )
        throw std::runtime_error("Failed to create Vulkan instance");
    */

    VULKAN_DEATH_CHECK(vkCreateInstance(&instance_create_info, nullptr, &_vk_instance), "Failed to create Vulkan instance");

    // Step 2: Create surface
    _window->_create_surface_private(_vk_instance);

    // Step 3: Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    uint32_t physical_device_count;
    vkEnumeratePhysicalDevices(_vk_instance, &physical_device_count, nullptr);

    std::cout << "Found " << physical_device_count << " physical devices";

    _physical_devices.reserve(physical_device_count);
    physical_devices.resize(physical_device_count);
    vkEnumeratePhysicalDevices(_vk_instance, &physical_device_count, physical_devices.data());

    for(size_t physical_device_index = 0; physical_device_index < physical_devices.size(); physical_device_index++)
        _physical_devices.push_back(new PhysicalDevice(this, physical_device_index, physical_devices[physical_device_index]));
}

DeviceContext::~DeviceContext(void)
{
    std::cout << "Destructing device context" << std::endl;

    for(size_t physical_device_index = 0; physical_device_index < _physical_devices.size(); physical_device_index++)
        delete _physical_devices[physical_device_index];

    delete _window;

    vkDestroyInstance(_vk_instance, nullptr);
}

VkInstance DeviceContext::get_instance(void) const
{
    return _vk_instance;
}

VkSurfaceKHR DeviceContext::get_surface(void) const
{
    return _window->_vk_surface;
}

LogicalDevice* DeviceContext::create_device(const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues)
{
    PhysicalDevice* chosen_physical_device = _find_physical_device(desired_features);
    _logical_device  = chosen_physical_device->create_logical_device(desired_queues);

    return _logical_device;
}

PhysicalDevice* DeviceContext::_find_physical_device(const VkPhysicalDeviceFeatures& features)
{
    for(PhysicalDevice* device : _physical_devices)
    {
        if(device->has_features(features))
            return device;
    }

    return nullptr;
}

