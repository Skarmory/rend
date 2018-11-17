#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vulkan.h>

namespace rend
{

class CommandPool;

class CommandBuffer
{
    friend class CommandPool;

public:

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&)      = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&)      = delete;

    void reset(void);

private:

    CommandBuffer(VkCommandBuffer vk_command_buffer, uint32_t index);
    ~CommandBuffer(void);

private:
    VkCommandBuffer _vk_command_buffer;
    uint32_t _index;
};

}

#endif
