#include "logical_device.h"

#include "physical_device.h"
#include "command_buffer.h"
#include "fence.h"
#include "semaphore.h"
#include "swapchain.h"

#include <array>
#include <set>

using namespace rend;

LogicalDevice::LogicalDevice(const DeviceContext& context)
    : _context(context),
      _physical_device(nullptr),
      _graphics_family(nullptr),
      _transfer_family(nullptr),
      _vk_device(VK_NULL_HANDLE),
      _vk_graphics_queue(VK_NULL_HANDLE),
      _vk_transfer_queue(VK_NULL_HANDLE)
{
}

bool LogicalDevice::create_logical_device(const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family)
{
    if(_vk_device != VK_NULL_HANDLE)
        return false;

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

    if(vkCreateDevice(physical_device->get_handle(), &device_create_info, nullptr, &_vk_device) != VK_SUCCESS)
        return false;

    // Step 3: Get queue handles
    if(graphics_family)
    {
        vkGetDeviceQueue(_vk_device, graphics_family->get_index(), 0, &_vk_graphics_queue);
        vkGetDeviceQueue(_vk_device, transfer_family->get_index(), 0, &_vk_transfer_queue);
    }

    _physical_device = physical_device;
    _graphics_family = graphics_family;
    _transfer_family = transfer_family;

    return true;
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

const DeviceContext& LogicalDevice::get_device_context(void) const
{
    return _context;
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

    for(CommandBuffer* buf : command_buffers)
        vk_command_buffers.push_back(buf->get_handle());

    for(Semaphore* sem : wait_sems)
        vk_wait_sems.push_back(sem->get_handle());

    for(Semaphore* sem : signal_sems)
        vk_sig_sems.push_back(sem->get_handle());

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

    if(vkQueueSubmit(queue, 1, &submit_info, fence->get_handle()) != VK_SUCCESS)
        return false;

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
        swapchain->get_handle(),
        timeout,
        semaphore ? semaphore->get_handle() : VK_NULL_HANDLE,
        fence ? fence->get_handle() : VK_NULL_HANDLE,
        image_index
    );
}

VkResult LogicalDevice::queue_present(QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Swapchain*>& swapchains, const std::vector<uint32_t>& image_indices, std::vector<VkResult>& results)
{
    std::vector<VkSwapchainKHR> vk_swapchains;
    std::vector<VkSemaphore> vk_sems;

    for(Swapchain* swapchain : swapchains)
        vk_swapchains.push_back(swapchain->get_handle());

    for(Semaphore* sem : wait_sems)
        vk_sems.push_back(sem->get_handle());

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

VkResult LogicalDevice::get_swapchain_images(Swapchain* swapchain, std::vector<VkImage>& images)
{
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(_vk_device, swapchain->get_handle(), &count, nullptr);

    images.resize(count);

    return vkGetSwapchainImagesKHR(_vk_device, swapchain->get_handle(), &count, images.data());
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
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts        = layouts.data()
    };

    std::vector<VkDescriptorSet> sets(layouts.size());
    vkAllocateDescriptorSets(_vk_device, &alloc_info, sets.data());

    return sets;
}

void LogicalDevice::update_descriptor_sets(std::vector<VkWriteDescriptorSet>& write_sets)
{
    vkUpdateDescriptorSets(_vk_device, write_sets.size(), write_sets.data(), 0, nullptr);
}

std::vector<VkCommandBuffer> LogicalDevice::allocate_command_buffers(uint32_t count, VkCommandBufferLevel level, VkCommandPool pool)
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
        return {};

    return vk_buffers;
}

void LogicalDevice::free_command_buffers(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool)
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

VkSwapchainKHR LogicalDevice::create_swapchain(
        VkSurfaceKHR surface, uint32_t min_image_count, VkFormat format,
        VkColorSpaceKHR colour_space, VkExtent2D extent, uint32_t array_layers,
        VkImageUsageFlags image_usage, VkSharingMode sharing_mode, uint32_t queue_family_index_count,
        const uint32_t* queue_family_indices, VkSurfaceTransformFlagBitsKHR pre_transform, VkCompositeAlphaFlagBitsKHR composite_alpha,
        VkPresentModeKHR present_mode, VkBool32 clipped, VkSwapchainKHR old_swapchain
    )
{
    VkSwapchainCreateInfoKHR create_info = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0,
        .surface               = surface,
        .minImageCount         = min_image_count,
        .imageFormat           = format,
        .imageColorSpace       = colour_space,
        .imageExtent           = extent,
        .imageArrayLayers      = array_layers,
        .imageUsage            = image_usage,
        .imageSharingMode      = sharing_mode,
        .queueFamilyIndexCount = queue_family_index_count,
        .pQueueFamilyIndices   = queue_family_indices,
        .preTransform          = pre_transform,
        .compositeAlpha        = composite_alpha,
        .presentMode           = present_mode,
        .clipped               = clipped,
        .oldSwapchain          = old_swapchain
    };

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(_vk_device, &create_info, nullptr, &swapchain);

    return swapchain;
}

void LogicalDevice::destroy_swapchain(VkSwapchainKHR swapchain)
{
    vkDestroySwapchainKHR(_vk_device, swapchain, nullptr);
}

VkRenderPass LogicalDevice::create_render_pass(std::vector<VkAttachmentDescription>& attach_descs, std::vector<VkSubpassDescription>& subpass_descs, std::vector<VkSubpassDependency>& subpass_deps)
{
    VkRenderPassCreateInfo create_info =
    {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .attachmentCount = static_cast<uint32_t>(attach_descs.size()),
        .pAttachments    = attach_descs.data(),
        .subpassCount    = static_cast<uint32_t>(subpass_descs.size()),
        .pSubpasses      = subpass_descs.data(),
        .dependencyCount = static_cast<uint32_t>(subpass_deps.size()),
        .pDependencies   = subpass_deps.data()
    };

    VkRenderPass render_pass = VK_NULL_HANDLE;
    vkCreateRenderPass(_vk_device, &create_info, nullptr, &render_pass);

    return render_pass;
}

void LogicalDevice::destroy_render_pass(VkRenderPass render_pass)
{
    vkDestroyRenderPass(_vk_device, render_pass, nullptr);
}

VkFramebuffer LogicalDevice::create_framebuffer(VkRenderPass render_pass, const std::vector<VkImageView>& attachments, uint32_t width, int32_t height, uint32_t layers)
{
    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.renderPass = render_pass;
    create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    create_info.pAttachments = attachments.data();
    create_info.width = width;
    create_info.height = height;
    create_info.layers = layers;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    vkCreateFramebuffer(_vk_device, &create_info, nullptr, &framebuffer);

    return framebuffer;
}

void LogicalDevice::destroy_framebuffer(VkFramebuffer framebuffer)
{
    vkDestroyFramebuffer(_vk_device, framebuffer, nullptr);
}

VkDescriptorPool LogicalDevice::create_descriptor_pool(uint32_t max_sets, std::vector<VkDescriptorPoolSize>& pool_sizes)
{
    VkDescriptorPoolCreateInfo create_info =
    {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .maxSets       = max_sets,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data()
    };

    VkDescriptorPool pool = VK_NULL_HANDLE;
    vkCreateDescriptorPool(_vk_device, &create_info, nullptr, &pool);

    return pool;
}

void LogicalDevice::destroy_descriptor_pool(VkDescriptorPool pool)
{
    vkDestroyDescriptorPool(_vk_device, pool, nullptr);
}

VkCommandPool LogicalDevice::create_command_pool(VkCommandPoolCreateInfo& create_info)
{
    VkCommandPool pool = VK_NULL_HANDLE;
    vkCreateCommandPool(_vk_device, &create_info, nullptr, &pool);
    return pool;
}

void LogicalDevice::destroy_command_pool(VkCommandPool pool)
{
    vkDestroyCommandPool(_vk_device, pool, nullptr);
}

VkDescriptorSetLayout LogicalDevice::create_descriptor_set_layout(std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    VkDescriptorSetLayoutCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()
    };

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

VkEvent LogicalDevice::create_event(void)
{
    VkEventCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;

    VkEvent event = VK_NULL_HANDLE;
    vkCreateEvent(_vk_device, &create_info, nullptr, &event);

    return event;
}

void LogicalDevice::destroy_event(VkEvent event)
{
    vkDestroyEvent(_vk_device, event, nullptr);
}

VkFence LogicalDevice::create_fence(VkFenceCreateInfo& create_info)
{
    VkFence fence = VK_NULL_HANDLE;
    vkCreateFence(_vk_device, &create_info, nullptr, &fence);
    return fence;
}

void LogicalDevice::destroy_fence(VkFence fence)
{
    vkDestroyFence(_vk_device, fence, nullptr);
}

VkSemaphore LogicalDevice::create_semaphore(VkSemaphoreCreateInfo& create_info)
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
