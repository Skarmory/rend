#ifndef REND_DEVICE_CONTEXT_H
#define REND_DEVICE_CONTEXT_H

#include "resource.h"

#include <vulkan.h>
#include <vector>

class GLFWwindow;

namespace rend
{

class PhysicalDevice;
class LogicalDevice;
class VulkanInstance;
class Window;

class DeviceContext : public core::Resource
{
public:
    explicit DeviceContext(VulkanInstance& instance);
    ~DeviceContext(void);

    DeviceContext(const DeviceContext&)            = delete;
    DeviceContext(DeviceContext&&)                 = delete;
    DeviceContext& operator=(const DeviceContext&) = delete;
    DeviceContext& operator=(DeviceContext&&)      = delete;

    PhysicalDevice* gpu(void) const;
    LogicalDevice*  get_device(void) const;
    Window*         get_window(void) const;

    StatusCode create(Window& window);
    void       destroy(void);

    StatusCode choose_gpu(const VkPhysicalDeviceFeatures& desired_features);
    StatusCode create_device(const VkQueueFlags desired_queues);

private:
    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

private:
    VulkanInstance& _instance;

    std::vector<PhysicalDevice*> _physical_devices;
    LogicalDevice*               _logical_device    { nullptr };
    PhysicalDevice*              _chosen_gpu        { nullptr };
    Window*                      _window            { nullptr };
};

}

#endif
