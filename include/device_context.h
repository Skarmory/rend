#ifndef DEVICE_CONTEXT_H
#define DEVICE_CONTEXT_H

#include <vulkan.h>
#include <vector>

class GLFWwindow;

namespace rend
{

class PhysicalDevice;
class LogicalDevice;

class DeviceContext
{
public:
    DeviceContext(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count, GLFWwindow* window);
    ~DeviceContext(void);

    DeviceContext(const DeviceContext&) = delete;
    DeviceContext(DeviceContext&&) = delete;

    DeviceContext& operator=(const DeviceContext&) = delete;
    DeviceContext& operator=(DeviceContext&&) = delete;

    PhysicalDevice* find_physical_device(const VkPhysicalDeviceFeatures& features);

private:
    VkInstance _vk_instance;
    VkSurfaceKHR _vk_surface;

    std::vector<PhysicalDevice*> _physical_devices;
    std::vector<LogicalDevice*> _logical_devices;
};

}

#endif
