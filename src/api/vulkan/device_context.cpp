#include "device_context.h"

#include "physical_device.h"
#include "window.h"

#include <GLFW/glfw3.h>

using namespace rend;

DeviceContext::DeviceContext(void)
    : _vk_instance(VK_NULL_HANDLE), _logical_device(nullptr), _window(nullptr)
{
}

DeviceContext::~DeviceContext(void)
{
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

LogicalDevice* DeviceContext::get_device(void) const
{
    return _logical_device;
}

bool DeviceContext::create_device_context(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count, Window* window)
{
    if(_vk_instance != VK_NULL_HANDLE)
        return false;

    _window = window;

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

    if(vkCreateInstance(&instance_create_info, nullptr, &_vk_instance) != VK_SUCCESS)
        return false;

    // Step 2: Create surface
    _window->_create_surface_private(_vk_instance);

    // Step 3: Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    uint32_t physical_device_count;
    vkEnumeratePhysicalDevices(_vk_instance, &physical_device_count, nullptr);

    _physical_devices.reserve(physical_device_count);
    physical_devices.resize(physical_device_count);
    vkEnumeratePhysicalDevices(_vk_instance, &physical_device_count, physical_devices.data());

    for(size_t physical_device_index = 0; physical_device_index < physical_devices.size(); physical_device_index++)
    {
        PhysicalDevice* pdev = new PhysicalDevice(this);
        pdev->create_physical_device(physical_device_index, physical_devices[physical_device_index]);
        _physical_devices.push_back(pdev);
    }

    return true;
}

bool DeviceContext::create_device(const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues)
{
    if(!_logical_device)
    {
        PhysicalDevice* chosen_physical_device = _find_physical_device(desired_features);
        if(chosen_physical_device->create_logical_device(desired_queues))
        {
            _logical_device = chosen_physical_device->get_logical_device();
            return true;
        }
    }

    return false;
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

