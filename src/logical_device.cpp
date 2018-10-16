#include "logical_device.h"

#include "command_pool.h"

#include <iostream>
#include <set>

using namespace rend;

LogicalDevice::LogicalDevice(VkPhysicalDevice vk_physical_device, const QueueFamily* const graphics_family, const QueueFamily* const present_family)
    : _vk_device(VK_NULL_HANDLE), _graphics_queue(VK_NULL_HANDLE), _present_queue(VK_NULL_HANDLE)
{
    std::cout << "Constructing logical device" << std::endl;

    // Step 1: Construct queue creation info
    float priority = 1.0f;
    uint32_t highest_queue_index = std::max(graphics_family->get_index(), present_family->get_index());

    std::set<uint32_t> unique_queue_families;
    if(graphics_family)
    {
        unique_queue_families.emplace(graphics_family->get_index());
        unique_queue_families.emplace(present_family->get_index());
    }

    std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;
    for(uint32_t queue_family : unique_queue_families)
    {
        device_queue_create_infos.push_back({
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queue_family, 
            .queueCount = 1,
            .pQueuePriorities = &priority 
        });
    }

    // Step 2: Create device 
    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size()),
        .pQueueCreateInfos = device_queue_create_infos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
        .pEnabledFeatures = nullptr
    };

    if(vkCreateDevice(vk_physical_device, &device_create_info, nullptr, &_vk_device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device");

    // Step 3: Get queue handles
    if(graphics_family)
    {
        vkGetDeviceQueue(_vk_device, graphics_family->get_index(), 0, &_graphics_queue);
        vkGetDeviceQueue(_vk_device, present_family->get_index(), 0, &_present_queue);

        _graphics_queue_index = graphics_family->get_index();
        _present_queue_index = present_family->get_index();
    }

    // Step 4: Create command pools
    _command_pools.resize(highest_queue_index + 1);
    for(uint32_t family : unique_queue_families)
    {
        _command_pools[family] = new CommandPool(_vk_device, family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT); 
    }
}

LogicalDevice::~LogicalDevice(void)
{
    std::cout << "Destructing logical device" << std::endl;

    for(CommandPool* pool : _command_pools)
    {
        if(pool)
            delete pool;
    }

    vkDestroyDevice(_vk_device, nullptr);
}

CommandPool& LogicalDevice::get_graphics_queue_command_pool(void) const
{
    if(_graphics_queue == VK_NULL_HANDLE)
        throw std::runtime_error("Attempted to get graphics queue command pool when device does not have a graphics queue");

    return *_command_pools[_graphics_queue_index];
}
