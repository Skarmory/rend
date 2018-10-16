#include "command_pool.h"

#include "command_buffer.h"
#include "queue_family.h"

#include <utility>
#include <iostream>

using namespace rend;

CommandPool::CommandPool(VkDevice vk_device, uint32_t queue_family, VkCommandPoolCreateFlagBits create_flags)
    : _vk_device(vk_device), _vk_command_pool(VK_NULL_HANDLE)
{
    std::cout << "Constructing command pool" << std::endl;

    VkCommandPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = create_flags,
        .queueFamilyIndex = queue_family
    };

    if(vkCreateCommandPool(vk_device, &create_info, nullptr, &_vk_command_pool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool");
}

CommandPool::~CommandPool(void)
{
    std::cout << "Destructing command pool" << std::endl;

    vkResetCommandPool(_vk_device, _vk_command_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

    vkDestroyCommandPool(_vk_device, _vk_command_pool, nullptr);
}

std::vector<CommandBuffer*> CommandPool::allocate_command_buffers(uint32_t count, bool primary)
{
    if(count == 0)
        throw std::runtime_error("Allocate command buffer called with count 0");

    std::vector<CommandBuffer*> buffers;
    buffers.reserve(count);

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = _vk_command_pool,
        .level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandBufferCount = count
    };

    size_t current_size = _vk_command_buffers.size();
    _vk_command_buffers.resize(current_size + count);
    VkCommandBuffer* range_start = &_vk_command_buffers[current_size];

    if(vkAllocateCommandBuffers(_vk_device, &alloc_info, range_start) != VK_SUCCESS)
       throw std::runtime_error("Failed to allocate command buffers"); 

    for(size_t index = current_size; index < _vk_command_buffers.size(); index++)
    {
        _command_buffers.push_back(CommandBuffer(_vk_command_buffers[index], index));
        buffers.push_back(&_command_buffers[index]);
    }

    return buffers;
}

void CommandPool::free_command_buffers(const std::vector<CommandBuffer*>& command_buffers)
{
    uint32_t swap_count = command_buffers.size();

    for(uint32_t count = swap_count; count > 0; count--)
    {
        uint32_t index = command_buffers[count]->_index;
        uint32_t swap_index = _command_buffers.size() - count;

       if(index != swap_index)
       {
            _command_buffers[swap_index]._index = index;

            std::swap(_command_buffers[index], _command_buffers[swap_index]);
            std::swap(_vk_command_buffers[index], _vk_command_buffers[swap_index]);
       }
    }

    vkFreeCommandBuffers(_vk_device, _vk_command_pool, swap_count, &_vk_command_buffers[_vk_command_buffers.size() - swap_count]);
    _vk_command_buffers.erase(_vk_command_buffers.end() - swap_count, _vk_command_buffers.end());
    _command_buffers.erase(_command_buffers.end() - swap_count, _command_buffers.end());
}
