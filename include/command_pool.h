#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class QueueFamily;
class CommandBuffer;

class CommandPool
{
public:
    CommandPool(VkDevice vk_device, uint32_t queue_family, VkCommandPoolCreateFlagBits create_flags);
    ~CommandPool(void);

    std::vector<CommandBuffer*> allocate_command_buffers(uint32_t count, bool primary=true); 
    CommandBuffer*              allocate_command_buffer(bool primary=true);
    void                        free_command_buffers(const std::vector<CommandBuffer*>& command_buffers);
    void                        free_command_buffer(CommandBuffer* command_buffer);

private:
    VkDevice _vk_device;
    VkCommandPool _vk_command_pool;

    std::vector<CommandBuffer*> _command_buffers;
    std::vector<VkCommandBuffer> _vk_command_buffers;
};

}

#endif
