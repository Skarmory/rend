#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;
class QueueFamily;
class CommandBuffer;

/*
 * Wrapper around a Vulkan command pool object that. Allocates and frees CommandBuffer objects.
 *
 * Objects of this type are created via the DeviceContext.
 *
 * Command buffers allocated by this pool MUST be freed by this pool.
 */
class CommandPool
{
    friend class LogicalDevice;

public:

    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;

    /*
     * Allocate a specified number of command buffers. These are primary command buffers by default.
     */
    std::vector<CommandBuffer*> allocate_command_buffers(uint32_t count, bool primary=true);

    /*
     * Allocate a single command buffer. This is a primary command buffer by default.
     */
    CommandBuffer* allocate_command_buffer(bool primary=true);

    /*
     * Efficiently free and destroy all the command buffers specified.
     */
    void free_command_buffers(const std::vector<CommandBuffer*>& command_buffers);

    /*
     * Efficiently free and destroy the command buffer specified.
     */
    void free_command_buffer(CommandBuffer* command_buffer);

    /*
     * Frees and destroys all command buffers allocated from this pool.
     */
    void free_all(void);

private:

    CommandPool(LogicalDevice* const logical_device, const QueueFamily& queue_family, bool can_reset);
    ~CommandPool(void);

private:
    VkCommandPool            _vk_command_pool;
    LogicalDevice* const     _logical_device;

    const QueueFamily* const _queue_family;
    const bool               _can_reset;

    std::vector<CommandBuffer*>  _command_buffers;
};

}

#endif
