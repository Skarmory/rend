#include "api/vulkan/logical_device.h"

#include "core/command_buffer.h"

#include "api/vulkan/extensions.h"
#include "api/vulkan/fence.h"
#include "api/vulkan/physical_device.h"
#include "api/vulkan/swapchain.h"
#include "api/vulkan/vulkan_semaphore.h"

#include <array>
#include <cassert>
#include <iostream>
#include <set>

using namespace rend;

LogicalDevice::LogicalDevice(const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family, const std::vector<DeviceFeature>& features)
{
    // Step 1: Construct queue creation info
    float priority = 1.0f;

    std::set<uint32_t> unique_queue_families;
    if(graphics_family)
    {
        unique_queue_families.emplace(graphics_family->get_index());
        unique_queue_families.emplace(transfer_family->get_index());
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

    std::array<const char*, 1> extensions =
    {
        vk::device_ext::khr::swapchain
    };

    PhysicalDeviceFeatures device_features = PhysicalDeviceFeatures::make_device_features(features);
    device_features.vk_1_1_features.pNext = &device_features.vk_1_2_features;
    device_features.vk_1_2_features.pNext = nullptr;

    VkPhysicalDeviceFeatures2 vk_device_features_2;
    vk_device_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vk_device_features_2.pNext = &device_features.vk_1_1_features;
    vk_device_features_2.features = device_features.vk_1_0_features;

    // Step 2: Create device 
    VkDeviceCreateInfo device_create_info =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &vk_device_features_2,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size()),
        .pQueueCreateInfos = device_queue_create_infos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr
    };

    VkResult result = vkCreateDevice(physical_device->get_handle(), &device_create_info, nullptr, &_vk_device);
    assert(result == VK_SUCCESS);

    // Step 3: Get queue handles
    if(graphics_family)
    {
        vkGetDeviceQueue(_vk_device, graphics_family->get_index(), 0, &_vk_graphics_queue);
        vkGetDeviceQueue(_vk_device, transfer_family->get_index(), 0, &_vk_transfer_queue);
    }

    _physical_device = physical_device;
    _graphics_family = graphics_family;
    _transfer_family = transfer_family;
}

LogicalDevice::~LogicalDevice(void)
{
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
    }

    return VK_NULL_HANDLE;
}

const QueueFamily* LogicalDevice::get_queue_family(QueueType type) const
{
    switch(type)
    {
        case QueueType::GRAPHICS: return _graphics_family;
        case QueueType::TRANSFER: return _transfer_family;
    }

    return nullptr;
}

const PhysicalDevice& LogicalDevice::get_physical_device(void) const
{
    return *_physical_device;
}

bool LogicalDevice::queue_submit(VkCommandBuffer* command_buffers, uint32_t command_buffers_count, QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Semaphore*>& signal_sems, const Fence* fence)
{
    //std::vector<VkCommandBuffer> vk_command_buffers;
    std::vector<VkSemaphore>     vk_wait_sems;
    std::vector<VkSemaphore>     vk_sig_sems;
    std::vector<VkPipelineStageFlags> vk_wait_stages;

    //vk_command_buffers.reserve(command_buffers.size());
    vk_wait_sems.reserve(wait_sems.size());
    vk_wait_stages.reserve(wait_sems.size());
    vk_sig_sems.reserve(signal_sems.size());

    //for(CommandBuffer* buf : command_buffers)
    //{
    //    vk_command_buffers.push_back(   );
    //}

    for(Semaphore* sem : wait_sems)
    {
        vk_wait_sems.push_back(sem->vk_handle());
        vk_wait_stages.push_back(sem->get_wait_stages());
    }

    for(Semaphore* sem : signal_sems)
    {
        vk_sig_sems.push_back(sem->vk_handle());
    }

    VkSubmitInfo submit_info =
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(vk_wait_sems.size()),
        .pWaitSemaphores = vk_wait_sems.data(),
        .pWaitDstStageMask = vk_wait_stages.data(),
        .commandBufferCount = command_buffers_count,
        .pCommandBuffers = command_buffers,
        .signalSemaphoreCount = static_cast<uint32_t>(vk_sig_sems.size()),
        .pSignalSemaphores = vk_sig_sems.data()
    };

    VkQueue queue = get_queue(type);

    if(vkQueueSubmit(queue, 1, &submit_info, fence ? fence->vk_handle() : VK_NULL_HANDLE) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

uint32_t LogicalDevice::find_memory_type(uint32_t desired_type, VkMemoryPropertyFlags memory_properties)
{
    const VkPhysicalDeviceMemoryProperties& properties = _physical_device->get_memory_properties();

    for(uint32_t idx = 0; idx < properties.memoryTypeCount; idx++)
    {
        bool required_type = desired_type & (1 << idx);
        bool required_prop = memory_properties & properties.memoryTypes[idx].propertyFlags;

        if(required_type && required_prop)
        {
            return idx;
        }
    }

    return std::numeric_limits<uint32_t>::max();
}

void LogicalDevice::wait_idle(void)
{
    vkDeviceWaitIdle(_vk_device);
}

VkResult LogicalDevice::wait_for_fences(std::vector<VkFence>& fences, uint64_t timeout, bool wait_all)
{
    return vkWaitForFences(_vk_device, fences.size(), fences.data(), wait_all, timeout);
}

void LogicalDevice::reset_fences(std::vector<VkFence>& fences)
{
    vkResetFences(_vk_device, fences.size(), fences.data());
}

VkResult LogicalDevice::acquire_next_image(Swapchain* swapchain, uint64_t timeout, Semaphore* semaphore, Fence* fence, uint32_t* image_index)
{
    return vkAcquireNextImageKHR(
        _vk_device,
        swapchain->vk_handle(),
        timeout,
        semaphore ? semaphore->vk_handle() : VK_NULL_HANDLE,
        fence ? fence->vk_handle() : VK_NULL_HANDLE,
        image_index
    );
}

VkResult LogicalDevice::queue_present(QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Swapchain*>& swapchains, const std::vector<uint32_t>& image_indices, std::vector<VkResult>& results)
{
    std::vector<VkSwapchainKHR> vk_swapchains;
    vk_swapchains.reserve(swapchains.size());

    std::vector<VkSemaphore> vk_sems;
    vk_sems.reserve(wait_sems.size());

    for(Swapchain* swapchain : swapchains)
    {
        vk_swapchains.push_back(swapchain->vk_handle());
    }

    for(Semaphore* sem : wait_sems)
    {
        vk_sems.push_back(sem->vk_handle());
    }

    VkPresentInfoKHR present_info =
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(vk_sems.size()),
        .pWaitSemaphores = vk_sems.data(),
        .swapchainCount = static_cast<uint32_t>(vk_swapchains.size()),
        .pSwapchains = vk_swapchains.data(),
        .pImageIndices = image_indices.data(),
        .pResults = !results.empty() ? results.data() : nullptr
    };

    return vkQueuePresentKHR(get_queue(type), &present_info);
}

VkResult LogicalDevice::get_swapchain_images(VkSwapchainKHR swapchain, std::vector<VkImage>& images)
{
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(_vk_device, swapchain, &count, nullptr);
    images.resize(count);
    return vkGetSwapchainImagesKHR(_vk_device, swapchain, &count, images.data());
}

VkMemoryRequirements LogicalDevice::get_buffer_memory_reqs(VkBuffer buffer)
{
    VkMemoryRequirements memory_reqs = {};
    vkGetBufferMemoryRequirements(_vk_device, buffer, &memory_reqs);
    return memory_reqs;
}

VkResult LogicalDevice::bind_buffer_memory(VkBuffer buffer, VkDeviceMemory memory)
{
    return vkBindBufferMemory(_vk_device, buffer, memory, 0);
}

VkMemoryRequirements LogicalDevice::get_image_memory_reqs(VkImage image)
{
    VkMemoryRequirements memory_reqs = {};
    vkGetImageMemoryRequirements(_vk_device, image, &memory_reqs);
    return memory_reqs;
}

VkResult LogicalDevice::bind_image_memory(VkImage image, VkDeviceMemory memory)
{
    return vkBindImageMemory(_vk_device, image, memory, 0);
}

std::vector<VkDescriptorSet> LogicalDevice::allocate_descriptor_sets(std::vector<VkDescriptorSetLayout>& layouts, VkDescriptorPool pool)
{
    VkDescriptorSetAllocateInfo alloc_info =
    {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = pool,
        .descriptorSetCount = (uint32_t)layouts.size(),
        .pSetLayouts        = layouts.data()
    };

    std::vector<VkDescriptorSet> sets(layouts.size());
    VkResult result = vkAllocateDescriptorSets(_vk_device, &alloc_info, sets.data());

    return sets;
}

void LogicalDevice::update_descriptor_sets(std::vector<VkWriteDescriptorSet>& write_sets)
{
    vkUpdateDescriptorSets(_vk_device, write_sets.size(), write_sets.data(), 0, nullptr);
}

void LogicalDevice::free_descriptor_sets(const VkDescriptorSet* sets, uint32_t sets_count, VkDescriptorPool pool)
{
    vkFreeDescriptorSets(_vk_device, pool, sets_count, sets);
}

std::vector<VkCommandBuffer> LogicalDevice::allocate_command_buffers(uint32_t count, VkCommandBufferLevel level, VkCommandPool pool) const
{
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.commandPool = pool;
    alloc_info.level = level;
    alloc_info.commandBufferCount = count;

    std::vector<VkCommandBuffer> vk_buffers;
    vk_buffers.resize(count);

    if(vkAllocateCommandBuffers(_vk_device, &alloc_info, vk_buffers.data()) != VK_SUCCESS)
    {
        return {};
    }

    return vk_buffers;
}

void LogicalDevice::free_command_buffers(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool) const
{
    vkFreeCommandBuffers(_vk_device, pool, buffers.size(), buffers.data());
}

VkDeviceMemory LogicalDevice::allocate_memory(VkMemoryAllocateInfo& alloc_info)
{
    VkDeviceMemory memory = VK_NULL_HANDLE;
    vkAllocateMemory(_vk_device, &alloc_info, nullptr, &memory);
    return memory;
}

void LogicalDevice::free_memory(VkDeviceMemory memory)
{
    vkFreeMemory(_vk_device, memory, nullptr);
}

bool LogicalDevice::map_memory(VkDeviceMemory memory, size_t size_bytes, uint64_t offset_bytes, void** mapped)
{
    auto code = vkMapMemory(_vk_device, memory, offset_bytes, size_bytes, 0, mapped);
    return code == VK_SUCCESS;
}

void LogicalDevice::unmap_memory(VkDeviceMemory memory)
{
    vkUnmapMemory(_vk_device, memory);
}

VkSwapchainKHR LogicalDevice::create_swapchain(VkSwapchainCreateInfoKHR& create_info)
{
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(_vk_device, &create_info, nullptr, &swapchain);
    return swapchain;
}

void LogicalDevice::destroy_swapchain(VkSwapchainKHR swapchain)
{
    vkDestroySwapchainKHR(_vk_device, swapchain, nullptr);
}

VkRenderPass LogicalDevice::create_render_pass(VkRenderPassCreateInfo& create_info)
{
    VkRenderPass render_pass = VK_NULL_HANDLE;
    vkCreateRenderPass(_vk_device, &create_info, nullptr, &render_pass);
    return render_pass;
}

void LogicalDevice::destroy_render_pass(VkRenderPass render_pass)
{
    vkDestroyRenderPass(_vk_device, render_pass, nullptr);
}

VkFramebuffer LogicalDevice::create_framebuffer(VkFramebufferCreateInfo& create_info)
{
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    vkCreateFramebuffer(_vk_device, &create_info, nullptr, &framebuffer);
    return framebuffer;
}

void LogicalDevice::destroy_framebuffer(VkFramebuffer framebuffer)
{
    vkDestroyFramebuffer(_vk_device, framebuffer, nullptr);
}

VkDescriptorPool LogicalDevice::create_descriptor_pool(VkDescriptorPoolCreateInfo& create_info)
{
    VkDescriptorPool pool = VK_NULL_HANDLE;
    vkCreateDescriptorPool(_vk_device, &create_info, nullptr, &pool);
    return pool;
}

void LogicalDevice::destroy_descriptor_pool(VkDescriptorPool pool)
{
    vkDestroyDescriptorPool(_vk_device, pool, nullptr);
}

VkCommandPool LogicalDevice::create_command_pool(VkCommandPoolCreateInfo& create_info) const
{
    VkCommandPool pool = VK_NULL_HANDLE;
    vkCreateCommandPool(_vk_device, &create_info, nullptr, &pool);
    return pool;
}

void LogicalDevice::destroy_command_pool(VkCommandPool pool) const
{
    vkDestroyCommandPool(_vk_device, pool, nullptr);
}

void LogicalDevice::reset_command_pool(VkCommandPool pool)
{
    vkResetCommandPool(_vk_device, pool, 0);
}

VkDescriptorSetLayout LogicalDevice::create_descriptor_set_layout(VkDescriptorSetLayoutCreateInfo& create_info)
{
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    vkCreateDescriptorSetLayout(_vk_device, &create_info, nullptr, &layout);
    return layout;
}

void LogicalDevice::destroy_descriptor_set_layout(VkDescriptorSetLayout layout)
{
    vkDestroyDescriptorSetLayout(_vk_device, layout, nullptr);
}

VkImageView LogicalDevice::create_image_view(VkImageViewCreateInfo& create_info)
{
    VkImageView image_view = VK_NULL_HANDLE;
    vkCreateImageView(_vk_device, &create_info, nullptr, &image_view);
    return image_view;
}

void LogicalDevice::destroy_image_view(VkImageView image_view)
{
    vkDestroyImageView(_vk_device, image_view, nullptr);
}

VkBuffer LogicalDevice::create_buffer(VkBufferCreateInfo& info)
{
    VkBuffer buffer = VK_NULL_HANDLE;
    vkCreateBuffer(_vk_device, &info, nullptr, &buffer);
    return buffer;
}

void LogicalDevice::destroy_buffer(VkBuffer buffer)
{
    vkDestroyBuffer(_vk_device, buffer, nullptr);
}

VkPipelineLayout LogicalDevice::create_pipeline_layout(VkPipelineLayoutCreateInfo& create_info)
{
    VkPipelineLayout layout = VK_NULL_HANDLE;
    vkCreatePipelineLayout(_vk_device, &create_info, nullptr, &layout);
    return layout;
}

void LogicalDevice::destroy_pipeline_layout(VkPipelineLayout layout)
{
    vkDestroyPipelineLayout(_vk_device, layout, nullptr);
}

VkPipeline LogicalDevice::create_pipeline(VkGraphicsPipelineCreateInfo& create_info)
{
    VkPipeline pipeline = VK_NULL_HANDLE;
    vkCreateGraphicsPipelines(_vk_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline);
    return pipeline;
}

void LogicalDevice::destroy_pipeline(VkPipeline pipeline)
{
    vkDestroyPipeline(_vk_device, pipeline, nullptr);
}

VkEvent LogicalDevice::create_event(const VkEventCreateInfo& create_info)
{
    VkEvent event = VK_NULL_HANDLE;
    vkCreateEvent(_vk_device, &create_info, nullptr, &event);
    return event;
}

void LogicalDevice::destroy_event(VkEvent event)
{
    vkDestroyEvent(_vk_device, event, nullptr);
}

VkFence LogicalDevice::create_fence(const VkFenceCreateInfo& create_info)
{
    VkFence fence = VK_NULL_HANDLE;
    vkCreateFence(_vk_device, &create_info, nullptr, &fence);
    return fence;
}

void LogicalDevice::destroy_fence(VkFence fence)
{
    vkDestroyFence(_vk_device, fence, nullptr);
}

VkSemaphore LogicalDevice::create_semaphore(const VkSemaphoreCreateInfo& create_info)
{
    VkSemaphore semaphore;
    vkCreateSemaphore(_vk_device, &create_info, nullptr, &semaphore);
    return semaphore;
}

void LogicalDevice::destroy_semaphore(VkSemaphore semaphore)
{
    vkDestroySemaphore(_vk_device, semaphore, nullptr);
}

VkImage LogicalDevice::create_image(VkImageCreateInfo& create_info)
{
    VkImage image = VK_NULL_HANDLE;
    vkCreateImage(_vk_device, &create_info, nullptr, &image);
    return image;
}

void LogicalDevice::destroy_image(VkImage image)
{
    vkDestroyImage(_vk_device, image, nullptr);
}

VkSampler LogicalDevice::create_sampler(VkSamplerCreateInfo& create_info)
{
    VkSampler sampler = VK_NULL_HANDLE;
    vkCreateSampler(_vk_device, &create_info, nullptr, &sampler);
    return sampler;
}

void LogicalDevice::destroy_sampler(VkSampler sampler)
{
    vkDestroySampler(_vk_device, sampler, nullptr);
}

VkShaderModule LogicalDevice::create_shader_module(VkShaderModuleCreateInfo& create_info)
{
    VkShaderModule module = VK_NULL_HANDLE;
    vkCreateShaderModule(_vk_device, &create_info, nullptr, &module);
    return module;
}

void LogicalDevice::destroy_shader_module(VkShaderModule module)
{
    vkDestroyShaderModule(_vk_device, module, nullptr);
}
