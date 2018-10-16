#include "command_buffer.h"

#include <iostream>

using namespace rend;

CommandBuffer::CommandBuffer(VkCommandBuffer vk_command_buffer, uint32_t index)
    : _vk_command_buffer(vk_command_buffer), _index(index)
{
    std::cout << "Constructing command buffer" << std::endl;
}

CommandBuffer::~CommandBuffer(void)
{
    std::cout << "Destructing command buffer" << std::endl;
}

void CommandBuffer::reset(void)
{
    if(vkResetCommandBuffer(_vk_command_buffer, 0) != VK_SUCCESS)
        throw std::runtime_error("Failed to reset command buffer");
}
