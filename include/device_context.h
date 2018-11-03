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
    class Key;

    DeviceContext(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count, Window* window);
    ~DeviceContext(void);

    DeviceContext(const DeviceContext&) = delete;
    DeviceContext(DeviceContext&&) = delete;

    DeviceContext& operator=(const DeviceContext&) = delete;
    DeviceContext& operator=(DeviceContext&&) = delete;

    PhysicalDevice* find_physical_device(const VkPhysicalDeviceFeatures& features);

    VkInstance get_instance(DeviceContext::Key key) const;
    VkSurfaceKHR get_surface(DeviceContext::Key key) const;

private:
    VkInstance _vk_instance;

    std::vector<PhysicalDevice*> _physical_devices;
    std::vector<LogicalDevice*> _logical_devices;

    Window* _window;
};

class DeviceContext::Key final
{
    friend class PhysicalDevice;
    friend class Swapchain;

    Key(void)  = default;
    ~Key(void) = default;

    Key(const Key&) = delete;
    Key(Key&&)      = delete;

    Key& operator=(const Key& key) = delete;
    Key& operator=(Key&& key)      = delete;
};

}

#endif
