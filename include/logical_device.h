#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H

#include "queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandPool;

class LogicalDevice
{
public:
    LogicalDevice(VkPhysicalDevice vk_physical_device, const QueueFamily* const graphics_family, const QueueFamily* const present_family);
    ~LogicalDevice(void);

    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&&) = delete;

    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

private:
    VkDevice _vk_device;
    VkQueue _graphics_queue;
    VkQueue _present_queue;

    std::vector<CommandPool*> _command_pools;
};

}

#endif

