#include "api/vulkan/command_pool.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/queue_family.h"
#include "api/vulkan/vulkan_helper_funcs.h"

using namespace rend;

bool CommandPool::create(const LogicalDevice& logical_device, QueueType type)
{
    VkCommandPoolCreateInfo create_info = vulkan_helpers::gen_command_pool_create_info();
    create_info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex        = logical_device.get_queue_family(type)->get_index();

    _vk_command_pool = logical_device.create_command_pool(create_info);
    if(_vk_command_pool == VK_NULL_HANDLE)
    {
        return false;
    }

    return true;
}

void CommandPool::destroy(const LogicalDevice& logical_device)
{
    {
        std::vector<VkCommandBuffer> vk_command_buffers;
        for(auto& handle : _vk_command_buffers)
        {
            vk_command_buffers.push_back(*_vk_command_buffers.get(handle));
        }

        logical_device.free_command_buffers(vk_command_buffers, _vk_command_pool);
    }

    _vk_command_buffers.clear();

    logical_device.destroy_command_pool(_vk_command_pool);
    _vk_command_pool = VK_NULL_HANDLE;
}

CommandBufferHandle CommandPool::allocate_command_buffer(const LogicalDevice& logical_device)
{
    VkCommandBuffer vk_command_buffer = logical_device.allocate_command_buffers(1, VK_COMMAND_BUFFER_LEVEL_PRIMARY, _vk_command_pool)[0];

    CommandBufferHandle handle = _vk_command_buffers.allocate(vk_command_buffer);

    return handle;
}

void CommandPool::free_command_buffer(const LogicalDevice& logical_device, CommandBufferHandle command_buffer_handle)
{
    std::vector<VkCommandBuffer> buffers;
    buffers.push_back(*_vk_command_buffers.get(command_buffer_handle));

    logical_device.free_command_buffers(buffers, _vk_command_pool);

    _vk_command_buffers.deallocate(command_buffer_handle);
}

VkCommandBuffer CommandPool::get_vk_command_buffer(CommandBufferHandle command_buffer_handle)
{
    return *_vk_command_buffers.get(command_buffer_handle);
}
