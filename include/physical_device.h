#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include "queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;

class PhysicalDevice
{
public:
    PhysicalDevice(uint32_t physical_device_index, VkPhysicalDevice physical_device, VkSurfaceKHR surface);
    ~PhysicalDevice(void);

    LogicalDevice* create_logical_device(VkQueueFlags queue_flags);

    uint32_t get_index(void) const;
    bool has_features(const VkPhysicalDeviceFeatures& features) const;
    bool has_queues(VkQueueFlags queue_flags) const;

private:
    uint32_t _physical_device_index;
    VkPhysicalDevice _vk_physical_device;
    VkPhysicalDeviceProperties _vk_physical_device_properties;
    VkPhysicalDeviceFeatures _vk_physical_device_features;

    std::vector<QueueFamily> _queue_families;
    std::vector<QueueFamily*> _graphics_queue_families;
    std::vector<QueueFamily*> _present_queue_families;

    LogicalDevice* _logical_device;
};

}

#endif

