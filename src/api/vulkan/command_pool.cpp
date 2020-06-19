#include "command_pool.h"

#include "command_buffer.h"
#include "device_context.h"
#include "logical_device.h"
#include "queue_family.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

#include <utility>
#include <iostream>

using namespace rend;

CommandPool::CommandPool(void)
    :
      _queue_family(nullptr),
      _can_reset(false),
      _vk_command_pool(VK_NULL_HANDLE)
{
}

CommandPool::~CommandPool(void)
{
    free_all();

    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->destroy_command_pool(_vk_command_pool);
}

bool CommandPool::create_command_pool(const QueueFamily* queue_family, bool can_reset)
{
    if(_vk_command_pool != VK_NULL_HANDLE)
        return false;

    VkCommandPoolCreateInfo create_info = vulkan_helpers::gen_command_pool_create_info();
    create_info.flags = static_cast<VkCommandPoolCreateFlags>(can_reset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0);
    create_info.queueFamilyIndex = queue_family->get_index();

    _vk_command_pool = static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->create_command_pool(create_info);
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

    VkCommandBufferLevel level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

    std::vector<VkCommandBuffer> vk_buffers = static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->allocate_command_buffers(count, level, _vk_command_pool);
    if(vk_buffers.empty())
        return {};

    std::vector<CommandBuffer*> buffers;
    buffers.reserve(count);

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
        static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->free_command_buffers(vk_buffers, _vk_command_pool);
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

    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->free_command_buffers(vk_buffers, _vk_command_pool);

    _command_buffers.clear();
}
