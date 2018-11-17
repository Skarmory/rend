#ifndef DEVICE_CONTEXT_H
#define DEVICE_CONTEXT_H

#include <vulkan.h>
#include <vector>

class GLFWwindow;

namespace rend
{

class PhysicalDevice;
class LogicalDevice;
class Window;

class DeviceContext
{
public:
    DeviceContext(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count, Window* window);
    ~DeviceContext(void);

    DeviceContext(const DeviceContext&) = delete;
    DeviceContext(DeviceContext&&)      = delete;

    DeviceContext& operator=(const DeviceContext&) = delete;
    DeviceContext& operator=(DeviceContext&&)      = delete;

    VkInstance   get_instance(void) const;
    VkSurfaceKHR get_surface(void) const;

    LogicalDevice* create_device(const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues);

private:
    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

private:
    VkInstance _vk_instance;

    std::vector<PhysicalDevice*> _physical_devices;
    LogicalDevice*               _logical_device;

    Window* _window;
};

}

#endif
