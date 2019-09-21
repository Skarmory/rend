#include "logical_device.h"

#include "physical_device.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "descriptor_pool.h"
#include "descriptor_set_layout.h"
#include "event.h"
#include "fence.h"
#include "framebuffer.h"
#include "gpu_buffer.h"
#include "image.h"
#include "pipeline.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "semaphore.h"
#include "shader.h"
#include "swapchain.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <stdexcept>
#include <set>

using namespace rend;

LogicalDevice::LogicalDevice(const DeviceContext* context, const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family)
    : _vk_device(VK_NULL_HANDLE),
      _vk_graphics_queue(VK_NULL_HANDLE),
      _vk_transfer_queue(VK_NULL_HANDLE),
      _context(context),
      _physical_device(physical_device),
      _graphics_family(graphics_family),
      _transfer_family(transfer_family)
{
}

bool LogicalDevice::create_logical_device(void)
{
    if(_vk_device != VK_NULL_HANDLE)
        return false;

    // Step 1: Construct queue creation info
    float priority = 1.0f;

    std::set<uint32_t> unique_queue_families;
    if(_graphics_family)
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
    if(_graphics_family)
    {
        vkGetDeviceQueue(_vk_device, _graphics_family->get_index(), 0, &_vk_graphics_queue);
        vkGetDeviceQueue(_vk_device, _transfer_family->get_index(), 0, &_vk_transfer_queue);
    }

    return true;
}

LogicalDevice::~LogicalDevice(void)
{
    for(CommandPool* pool : _command_pools)
    {
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

const DeviceContext& LogicalDevice::get_device_context(void) const
{
    return *_context;
}

const PhysicalDevice& LogicalDevice::get_physical_device(void) const
{
    return *_physical_device;
}

bool LogicalDevice::queue_submit(const std::vector<CommandBuffer*>& command_buffers, QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Semaphore*>& signal_sems, Fence* fence)
{
    std::vector<VkCommandBuffer> vk_command_buffers;
    std::vector<VkSemaphore>     vk_wait_sems;
    std::vector<VkSemaphore>     vk_sig_sems;

    vk_command_buffers.reserve(command_buffers.size());
    vk_wait_sems.reserve(wait_sems.size());
    vk_sig_sems.reserve(signal_sems.size());

    std::for_each(command_buffers.begin(), command_buffers.end(), [&vk_command_buffers](CommandBuffer* buf){ vk_command_buffers.push_back(buf->get_handle());  });
    std::for_each(wait_sems.begin(), wait_sems.end(), [&vk_wait_sems](Semaphore* s){ vk_wait_sems.push_back(s->get_handle());  });
    std::for_each(signal_sems.begin(), signal_sems.end(), [&vk_sig_sems](Semaphore* s){ vk_sig_sems.push_back(s->get_handle());  });

    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(vk_wait_sems.size()),
        .pWaitSemaphores = vk_wait_sems.data(),
        .pWaitDstStageMask = &wait_stages,
        .commandBufferCount = static_cast<uint32_t>(command_buffers.size()),
        .pCommandBuffers = vk_command_buffers.data(),
        .signalSemaphoreCount = static_cast<uint32_t>(vk_sig_sems.size()),
        .pSignalSemaphores = vk_sig_sems.data()
    };

    VkQueue queue = get_queue(type);

    VULKAN_DEATH_CHECK(vkQueueSubmit(queue, 1, &submit_info, fence->get_handle()), "Failed to submit queue");

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
            return idx;
    }

    return std::numeric_limits<uint32_t>::max();
}

RenderPass* LogicalDevice::create_render_pass(const std::vector<VkAttachmentDescription>& attachment_descs, const std::vector<VkSubpassDescription>& subpass_descs, const std::vector<VkSubpassDependency>& subpass_deps)
{
    RenderPass* render_pass = new RenderPass(this, attachment_descs, subpass_descs, subpass_deps);

    return render_pass;
}

void LogicalDevice::destroy_render_pass(RenderPass** render_pass)
{
    if(render_pass && *render_pass)
    {
        delete (*render_pass);
        *render_pass = nullptr;
    }
}

Framebuffer* LogicalDevice::create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& image_views, VkExtent3D dimensions)
{
    Framebuffer* framebuffer = new Framebuffer(this, render_pass, image_views, dimensions);

    return framebuffer;
}

void LogicalDevice::destroy_framebuffer(Framebuffer** framebuffer)
{
    if(framebuffer && *framebuffer)
    {
        delete (*framebuffer);
        *framebuffer = nullptr;
    }
}

DescriptorPool* LogicalDevice::create_descriptor_pool(uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& pool_sizes)
{
    DescriptorPool* pool = new DescriptorPool(this, max_sets, pool_sizes);

    return pool;
}

void LogicalDevice::destroy_descriptor_pool(DescriptorPool** pool)
{
    if(pool && *pool)
    {
        delete (*pool);
        *pool = nullptr;
    }
}

DescriptorSetLayout* LogicalDevice::create_descriptor_set_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    DescriptorSetLayout* layout = new DescriptorSetLayout(this, bindings);

    return layout;
}

void LogicalDevice::destroy_descriptor_set_layout(DescriptorSetLayout** layout)
{
    if(layout && *layout)
    {
        delete (*layout);
        *layout = nullptr;
    }
}

PipelineLayout* LogicalDevice::create_pipeline_layout(const std::vector<DescriptorSetLayout*>& desc_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges)
{
    PipelineLayout* layout = new PipelineLayout(this, desc_set_layouts, push_constant_ranges);

    return layout;
}

void LogicalDevice::destroy_pipeline_layout(PipelineLayout** layout)
{
    if(layout && *layout)
    {
        delete (*layout);
        *layout = nullptr;
    }
}

Pipeline* LogicalDevice::create_pipeline(PipelineSettings* settings)
{
    Pipeline* pipeline = new Pipeline(this, settings);

    return pipeline;
}

void LogicalDevice::destroy_pipeline(Pipeline** pipeline)
{
    if(pipeline && *pipeline)
    {
        delete (*pipeline);
        *pipeline = nullptr;
    }
}

Shader* LogicalDevice::create_shader(const void* data, uint32_t size_bytes, ShaderType type)
{
    Shader* shader = new Shader(this, data, size_bytes, type);

    return shader;
}

void LogicalDevice::destroy_shader(Shader** shader)
{
    if(shader && *shader)
    {
        delete (*shader);
        *shader = nullptr;
    }
}

Semaphore* LogicalDevice::create_semaphore(void)
{
    Semaphore* semaphore = new Semaphore(this);

    return semaphore;
}

void LogicalDevice::destroy_semaphore(Semaphore** semaphore)
{
    if(semaphore && *semaphore)
    {
        delete (*semaphore);
        *semaphore = nullptr;
    }
}

Fence* LogicalDevice::create_fence(bool start_signalled)
{
    Fence* fence = new Fence(this, start_signalled);

    return fence;
}

void LogicalDevice::destroy_fence(Fence** fence)
{
    if(fence && *fence)
    {
        delete (*fence);
        *fence = nullptr;
    }
}

Event* LogicalDevice::create_event(void)
{
    Event* event = new Event(this);

    return event;
}

void LogicalDevice::destroy_event(Event** event)
{
    if(event && *event)
    {
        delete (*event);
        *event = nullptr;
    }
}

Image* LogicalDevice::create_image(VkExtent3D extent, VkImageType type, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspects)
{
    Image* image = new Image(this, extent, type, format, mip_levels, array_layers, samples, tiling, memory_properties, usage, view_type, aspects);

    return image;
}

void LogicalDevice::destroy_image(Image** image)
{
    if(image && *image)
    {
        delete (*image);
        *image = nullptr;
    }
}
