#include "device_context.h"

#include "physical_device.h"
#include "window.h"
#include "vulkan_instance.h"

#include <GLFW/glfw3.h>

using namespace rend;

DeviceContext::DeviceContext(VulkanInstance& instance)
    : _instance(instance),
      _logical_device(nullptr)
{
}

DeviceContext::~DeviceContext(void)
{
    for(size_t physical_device_index = 0; physical_device_index < _physical_devices.size(); physical_device_index++)
        delete _physical_devices[physical_device_index];
}

LogicalDevice* DeviceContext::get_device(void) const
{
    return _logical_device;
}

Window* DeviceContext::get_window(void) const
{
    return _window;
}

bool DeviceContext::create_device_context(Window* window)
{
    _window = window;

    // Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    _instance.enumerate_physical_devices(physical_devices);

    for(size_t physical_device_index = 0; physical_device_index < physical_devices.size(); physical_device_index++)
    {
        PhysicalDevice* pdev = new PhysicalDevice(*this);
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
