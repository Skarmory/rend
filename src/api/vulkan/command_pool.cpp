#include "command_pool.h"

#include "command_buffer.h"
#include "device_context.h"
#include "logical_device.h"
#include "queue_family.h"
#include "utils.h"

#include <utility>
#include <iostream>

using namespace rend;

CommandPool::CommandPool(DeviceContext* context)
    : _vk_command_pool(VK_NULL_HANDLE),
      _context(context),
      _queue_family(nullptr),
      _can_reset(false)
{
}

CommandPool::~CommandPool(void)
{
    vkDestroyCommandPool(_context->get_device()->get_handle(), _vk_command_pool, nullptr);
}

bool CommandPool::create_command_pool(const QueueFamily* queue_family, bool can_reset)
{
    if(_vk_command_pool != VK_NULL_HANDLE)
        return false;

    VkCommandPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkCommandPoolCreateFlags>(can_reset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0),
        .queueFamilyIndex = queue_family->get_index()
    };

    if(vkCreateCommandPool(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_command_pool) != VK_SUCCESS)
        return false;

    _queue_family = queue_family;
    _can_reset = can_reset;

    return true;
}

std::vector<CommandBuffer*> CommandPool::allocate_command_buffers(uint32_t count, bool primary)
{
    DEATH_CHECK(count == 0, "Allocate command buffer called with count 0");

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = _vk_command_pool,
        .level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandBufferCount = count
    };

    std::vector<VkCommandBuffer> vk_buffers;
    vk_buffers.reserve(count);

    std::vector<CommandBuffer*> buffers;
    buffers.reserve(count);

    VULKAN_DEATH_CHECK(vkAllocateCommandBuffers(_context->get_device()->get_handle(), &alloc_info, vk_buffers.data()), "Failed to allocate command buffers");

    _command_buffers.reserve(_command_buffers.size() + count);

    for(size_t i = 0; i < count; i++)
    {
        _command_buffers.push_back(new CommandBuffer(vk_buffers[i], _command_buffers.size() + i));
        buffers.push_back(_command_buffers.back());
    }

    return buffers;
}

CommandBuffer* CommandPool::allocate_command_buffer(bool primary)
{
    return allocate_command_buffers(1, primary)[0];
}

void CommandPool::free_command_buffers(const std::vector<CommandBuffer*>& command_buffers)
{
    size_t swap_count = command_buffers.size();

    std::vector<VkCommandBuffer> vk_buffers;
    vk_buffers.reserve(swap_count);

    for(size_t count = swap_count; count > 0; count--)
    {
        size_t index_from = command_buffers[count - 1]->_index;
        size_t index_to = _command_buffers.size() - count;

        vk_buffers.push_back(_command_buffers[index_from]->_vk_command_buffer);

        if(index_from != index_to)
        {
           // Set index of element being swapped with
            _command_buffers[index_to]->_index = index_from;

            std::swap(_command_buffers[index_from], _command_buffers[index_to]);
        }

    }

    vkFreeCommandBuffers(_context->get_device()->get_handle(), _vk_command_pool, swap_count, vk_buffers.data());

    _command_buffers.erase(_command_buffers.end() - swap_count, _command_buffers.end());
}

void CommandPool::free_command_buffer(CommandBuffer* command_buffer)
{
    free_command_buffers({ command_buffer });
}

void CommandPool::free_all(void)
{
    std::vector<VkCommandBuffer> vk_buffers;
    vk_buffers.reserve(_command_buffers.size());

    for(CommandBuffer* buffer : _command_buffers)
    {
        vk_buffers.push_back(buffer->_vk_command_buffer);
        delete buffer;
    }

    vkFreeCommandBuffers(_context->get_device()->get_handle(), _vk_command_pool, vk_buffers.size(), vk_buffers.data());

    _command_buffers.clear();
}
