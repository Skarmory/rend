#ifndef REND_PHYSICAL_DEVICE_H
#define REND_PHYSICAL_DEVICE_H

#include "queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;
class Window;

class PhysicalDevice
{
public:
    PhysicalDevice(void)                             = default;
    ~PhysicalDevice(void)                            = default;
    PhysicalDevice(const PhysicalDevice&)            = delete;
    PhysicalDevice(PhysicalDevice&&)                 = delete;
    PhysicalDevice& operator=(const PhysicalDevice&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&&)      = delete;

    bool create(uint32_t physical_device_index, VkPhysicalDevice physical_device);
    void destroy(void);

    bool create_logical_device(const VkQueueFlags queue_flags);

    LogicalDevice*                          get_logical_device(void) const;
    uint32_t                                get_index(void) const;
    VkPhysicalDevice                        get_handle(void) const;
    const std::vector<VkSurfaceFormatKHR>&  get_surface_formats(void) const;
    const std::vector<VkPresentModeKHR>&    get_surface_present_modes(void) const;
    VkSurfaceCapabilitiesKHR                get_surface_capabilities(void) const;
    const VkPhysicalDeviceMemoryProperties& get_memory_properties(void) const;

    bool has_features(const VkPhysicalDeviceFeatures& features) const;
    bool has_queues(VkQueueFlags queue_flags) const;

private:
    bool _find_queue_families(VkSurfaceKHR surface);
    bool _find_surface_formats(VkSurfaceKHR surface);
    bool _find_surface_present_modes(VkSurfaceKHR surface);

private:
    LogicalDevice*                   _logical_device{ nullptr };

    int32_t                          _physical_device_index{ -1 };
    VkPhysicalDevice                 _vk_physical_device{ VK_NULL_HANDLE };
    VkPhysicalDeviceProperties       _vk_physical_device_properties{};
    VkPhysicalDeviceFeatures         _vk_physical_device_features{};
    VkPhysicalDeviceMemoryProperties _vk_physical_device_memory_properties{};

    std::vector<VkSurfaceFormatKHR>  _vk_surface_formats;
    std::vector<VkPresentModeKHR>    _vk_present_modes;

    std::vector<QueueFamily>         _queue_families;
    std::vector<QueueFamily*>        _graphics_queue_families;
    std::vector<QueueFamily*>        _present_queue_families;
};

}

#endif
