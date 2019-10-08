#ifndef REND_DEVICE_CONTEXT_H
#define REND_DEVICE_CONTEXT_H

#include <vulkan.h>
#include <vector>

class GLFWwindow;

namespace rend
{

class PhysicalDevice;
class LogicalDevice;
class VulkanInstance;
class Window;

class DeviceContext
{
public:
    explicit DeviceContext(VulkanInstance& instance);
    ~DeviceContext(void);

    DeviceContext(const DeviceContext&)            = delete;
    DeviceContext(DeviceContext&&)                 = delete;
    DeviceContext& operator=(const DeviceContext&) = delete;
    DeviceContext& operator=(DeviceContext&&)      = delete;

    LogicalDevice* get_device(void) const;
    Window*        get_window(void) const;

    bool create_device_context(Window* window);
    bool create_device(const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues);

private:
    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

private:
    VulkanInstance& _instance;

    std::vector<PhysicalDevice*> _physical_devices;
    LogicalDevice*               _logical_device;
    Window*                      _window;
};

}

#endif
