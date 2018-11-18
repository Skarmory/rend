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

    LogicalDevice(const DeviceContext* device_context, const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family);
    ~LogicalDevice(void);

    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&&) = delete;

    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

    // Retrieval
    const DeviceContext&     get_device_context(void) const;
    const PhysicalDevice&    get_physical_device(void) const;
    const QueueFamily* const get_graphics_queue_family(void) const;
    const QueueFamily* const get_transfer_queue_family(void) const;
    VkDevice                 get_handle(void) const;

    // Creational
    CommandPool*             create_command_pool(const QueueType type, bool can_reset);
    void                     destroy_command_pool(CommandPool** command_pool);

    Swapchain*               create_swapchain(uint32_t desired_images);
    void                     destroy_swapchain(Swapchain** swapchain);

private:
    VkDevice _vk_device;
    VkQueue _graphics_queue;
    VkQueue _transfer_queue;

    const DeviceContext* _context;
    const PhysicalDevice* _physical_device;

    const QueueFamily* const _graphics_family;
    const QueueFamily* const _transfer_family;

    std::vector<CommandPool*> _command_pools;
};

}

#endif

