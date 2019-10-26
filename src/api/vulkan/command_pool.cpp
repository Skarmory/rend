#include "command_pool.h"

#include "command_buffer.h"
#include "device_context.h"
#include "logical_device.h"
#include "queue_family.h"

#include <utility>
#include <iostream>

using namespace rend;

CommandPool::CommandPool(DeviceContext& context)
    : _context(context),
      _queue_family(nullptr),
      _can_reset(false),
      _vk_command_pool(VK_NULL_HANDLE)
{
}

CommandPool::~CommandPool(void)
{
    free_all();

    _context.get_device()->destroy_command_pool(_vk_command_pool);
}

bool CommandPool::create_command_pool(const QueueFamily* queue_family, bool can_reset)
{
    if(_vk_command_pool != VK_NULL_HANDLE)
        return false;

    _vk_command_pool = _context.get_device()->create_command_pool(can_reset, queue_family->get_index());
    if(_vk_command_pool == VK_NULL_HANDLE)
        return false;

    _queue_family = queue_family;
    _can_reset = can_reset;

    return true;
}

std::vector<CommandBuffer*> CommandPool::allocate_command_buffers(uint32_t count, bool primary)
{
    if(count == 0)
        return {};

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

    if(vkAllocateCommandBuffers(_context.get_device()->get_handle(), &alloc_info, vk_buffers.data()) != VK_SUCCESS)
        return {};

    _command_buffers.reserve(_command_buffers.size() + count);

    for(size_t i = 0; i < count; i++)
    {
        _command_buffers.push_back(new CommandBuffer(this, vk_buffers[i]));
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
    std::vector<VkCommandBuffer> vk_buffers;
    vk_buffers.reserve(command_buffers.size());

    for(CommandBuffer* buf : command_buffers)
    {
        auto it = std::remove(_command_buffers.begin(), _command_buffers.end(), buf);
        if(it != _command_buffers.end())
        {
            _command_buffers.erase(it);
            vk_buffers.push_back(buf->get_handle());
            delete buf;
        }
    }

    if(vk_buffers.size() > 0)
        vkFreeCommandBuffers(_context.get_device()->get_handle(), _vk_command_pool, vk_buffers.size(), vk_buffers.data());
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
        vk_buffers.push_back(buffer->get_handle());
        delete buffer;
    }

    vkFreeCommandBuffers(_context.get_device()->get_handle(), _vk_command_pool, vk_buffers.size(), vk_buffers.data());

    _command_buffers.clear();
}
