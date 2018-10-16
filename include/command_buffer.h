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
    CommandBuffer(VkCommandBuffer vk_command_buffer, uint32_t index);
    ~CommandBuffer(void);

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    
    CommandBuffer(CommandBuffer&& other) = default;
    CommandBuffer& operator=(CommandBuffer&& other) = default;

    void reset(void);

private:

private:
    VkCommandBuffer _vk_command_buffer;
    uint32_t _index;
};

}

#endif
