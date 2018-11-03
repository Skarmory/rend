#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H

#include "queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class DeviceContext;
class PhysicalDevice;
class CommandPool;
class CommandBuffer;
class Swapchain;

class LogicalDevice
{

public:
    class Key;

    LogicalDevice(const DeviceContext* device_context, const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const present_family);
    ~LogicalDevice(void);

    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&&) = delete;

    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

    const DeviceContext&     get_device_context(void) const;
    const PhysicalDevice&    get_physical_device(void) const;
    const QueueFamily* const get_graphics_queue_family(void) const;
    const QueueFamily* const get_present_queue_family(void) const;
    CommandPool&             get_graphics_queue_command_pool(void) const;
    VkDevice                 get_handle(LogicalDevice::Key key) const;

private:
    VkDevice _vk_device;
    VkQueue _graphics_queue;
    VkQueue _present_queue;

    const DeviceContext* _context;
    const PhysicalDevice* _physical_device;

    const QueueFamily* const _graphics_family;
    const QueueFamily* const _present_family;

    std::vector<CommandPool*> _command_pools;
};

class LogicalDevice::Key
{
    friend class Swapchain;

private:

    Key(void) = default;
    ~Key(void) = default;
    Key(const Key&) = delete;
    Key(Key&&) = delete;
    Key& operator=(const Key&) = delete;
    Key& operator=(Key&&) = delete;
};

}


#endif

