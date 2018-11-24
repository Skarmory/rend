#include "logical_device.h"

#include "physical_device.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "swapchain.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <stdexcept>
#include <set>

using namespace rend;

LogicalDevice::LogicalDevice(const DeviceContext* context, const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family)
    : _vk_device(VK_NULL_HANDLE), _vk_graphics_queue(VK_NULL_HANDLE), _vk_transfer_queue(VK_NULL_HANDLE), _context(context),  _physical_device(physical_device), _graphics_family(graphics_family), _transfer_family(transfer_family)
{
    std::cout << "Constructing logical device" << std::endl;

    // Step 1: Construct queue creation info
    float priority = 1.0f;

    std::set<uint32_t> unique_queue_families;
    if(graphics_family)
    {
        unique_queue_families.emplace(_graphics_family->get_index());
        unique_queue_families.emplace(_transfer_family->get_index());
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

    std::array<const char*, 1> extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Step 2: Create device 
    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size()),
        .pQueueCreateInfos = device_queue_create_infos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr
    };

    VULKAN_DEATH_CHECK(vkCreateDevice(_physical_device->get_handle(), &device_create_info, nullptr, &_vk_device), "Failed to create logical device");

    // Step 3: Get queue handles
    if(graphics_family)
    {
        vkGetDeviceQueue(_vk_device, _graphics_family->get_index(), 0, &_vk_graphics_queue);
        vkGetDeviceQueue(_vk_device, _transfer_family->get_index(), 0, &_vk_transfer_queue);
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

VkDevice LogicalDevice::get_handle(void) const
{
    return _vk_device;
}

VkQueue LogicalDevice::get_queue(QueueType type) const
{
    switch(type)
    {
        case QueueType::GRAPHICS: return _vk_graphics_queue;
        case QueueType::TRANSFER: return _vk_transfer_queue;
        default:
            std::cerr << "Invalid queue type given in LogicalDevice::get_queue" << std::endl;
            std::abort();
    }

    return VK_NULL_HANDLE;
}

const DeviceContext& LogicalDevice::get_device_context(void) const
{
    return *_context;
}

const PhysicalDevice& LogicalDevice::get_physical_device(void) const
{
    return *_physical_device;
}

bool LogicalDevice::queue_submit(const std::vector<CommandBuffer*>& command_buffers, QueueType type, const std::vector<VkSemaphore>& wait_sems, const std::vector<VkSemaphore>& signal_sems, VkFence fence)
{
    std::vector<VkCommandBuffer> vk_command_buffers;
    vk_command_buffers.reserve(command_buffers.size());

    std::for_each(command_buffers.begin(), command_buffers.end(), [&vk_command_buffers](CommandBuffer* buf){ vk_command_buffers.push_back(buf->get_handle());  });

    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(wait_sems.size()),
        .pWaitSemaphores = wait_sems.data(),
        .pWaitDstStageMask = &wait_stages,
        .commandBufferCount = static_cast<uint32_t>(command_buffers.size()),
        .pCommandBuffers = vk_command_buffers.data(),
        .signalSemaphoreCount = static_cast<uint32_t>(signal_sems.size()),
        .pSignalSemaphores = signal_sems.data()
    };

    VkQueue queue = get_queue(type);

    VULKAN_DEATH_CHECK(vkQueueSubmit(queue, 1, &submit_info, fence), "Failed to submit queue");

    return true;
}

CommandPool* LogicalDevice::create_command_pool(const QueueType type, bool can_reset)
{
    switch(type)
    {
        case QueueType::GRAPHICS:
            _command_pools.push_back(new CommandPool(this, *_graphics_family, can_reset));
            break;
        case QueueType::TRANSFER:
            _command_pools.push_back(new CommandPool(this, *_transfer_family, can_reset));
            break;
        default:
            DEATH_CHECK(true, "Unknown queue type specified");
    }

    return _command_pools.back();
}

void LogicalDevice::destroy_command_pool(CommandPool** command_pool)
{
    if(!command_pool || !(*command_pool))
        return;

    auto it = std::find(_command_pools.begin(), _command_pools.end(), *command_pool);
    if(it == _command_pools.end())
        return;  // Not allocated by this device

    std::iter_swap(it, _command_pools.end() - 1);
    _command_pools.pop_back();

    (*command_pool)->free_all();

    delete (*command_pool);

    *command_pool = nullptr;
}

Swapchain* LogicalDevice::create_swapchain(uint32_t desired_images)
{
    Swapchain* swapchain = new Swapchain(this, desired_images);

    return swapchain;
}

void LogicalDevice::destroy_swapchain(Swapchain** swapchain)
{
    if(!swapchain || !*swapchain)
       return;

    delete (*swapchain);
    *swapchain = nullptr;
}
