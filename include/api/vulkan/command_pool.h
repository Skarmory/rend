#ifndef REND_COMMAND_POOL_H
#define REND_COMMAND_POOL_H

#include "core/data_structures/data_array.h"
#include "core/rend_defs.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;
enum class QueueType;

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
    CommandPool(void)                          = default;
    ~CommandPool(void)                         = default;
    CommandPool(const CommandPool&)            = delete;
    CommandPool(CommandPool&&)                 = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&&)      = delete;

    bool create(const LogicalDevice& logical_device, QueueType type);
    void destroy(const LogicalDevice& logical_device);

    CommandBufferHandle allocate_command_buffer(const LogicalDevice& logical_device);
    void                free_command_buffer(const LogicalDevice& logical_device, CommandBufferHandle command_buffer);
    VkCommandBuffer     get_vk_command_buffer(CommandBufferHandle command_buffer_handle);

private:
    DataArray<VkCommandBuffer>  _vk_command_buffers;
    VkCommandPool               _vk_command_pool{ VK_NULL_HANDLE };
};

}

#endif
