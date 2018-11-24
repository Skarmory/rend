#include "command_buffer.h"

#include "utils.h"

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

VkCommandBuffer CommandBuffer::get_handle(void) const
{
    return _vk_command_buffer;
}

void CommandBuffer::begin(void)
{
    VkCommandBufferBeginInfo info =
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .pInheritanceInfo = nullptr
    };

    VULKAN_DEATH_CHECK(vkBeginCommandBuffer(_vk_command_buffer, &info), "Failed to begin command buffer");
}

void CommandBuffer::end(void)
{
    vkEndCommandBuffer(_vk_command_buffer);
}

void CommandBuffer::reset(void)
{
    VULKAN_DEATH_CHECK(vkResetCommandBuffer(_vk_command_buffer, 0), "Failed to reset command buffer");
}
