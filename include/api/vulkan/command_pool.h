#ifndef REND_COMMAND_POOL_H
#define REND_COMMAND_POOL_H

#include <vulkan.h>
#include <vector>

namespace rend
{

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
public:
    CommandPool(void) = default;
    ~CommandPool(void) = default;

    CommandPool(const CommandPool&)            = delete;
    CommandPool(CommandPool&&)                 = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&&)      = delete;

    bool create(const QueueFamily* queue_family, bool can_reset);
    void destroy(void);

    /*
     * Allocate a specified number of command buffers. These are primary command buffers by default.
     */
    std::vector<CommandBuffer*> allocate_command_buffers(uint32_t count, bool primary=true);

    /*
     * Allocate a single command buffer. This is a primary command buffer by default.
     */
    CommandBuffer* allocate_command_buffer(bool primary=true);

    /*
     * Free and destroy all the command buffers specified.
     */
    void free_command_buffers(const std::vector<CommandBuffer*>& command_buffers);

    /*
     * Free and destroy the command buffer specified.
     */
    void free_command_buffer(CommandBuffer* command_buffer);

    /*
     * Frees and destroys all command buffers allocated from this pool.
     */
    void free_all(void);

private:
    const QueueFamily*          _queue_family{ nullptr };
    bool                        _can_reset{ false };
    std::vector<CommandBuffer*> _command_buffers;
    VkCommandPool               _vk_command_pool{ VK_NULL_HANDLE };
};

}

#endif
