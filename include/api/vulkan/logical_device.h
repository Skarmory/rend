#ifndef REND_LOGICAL_DEVICE_H
#define REND_LOGICAL_DEVICE_H

#include "core/rend_defs.h"

#include "api/vulkan/queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandBuffer;
class Fence;
class PhysicalDevice;
class Semaphore;
class Swapchain;

class LogicalDevice
{

public:
    LogicalDevice(void)                            = default;
    ~LogicalDevice(void)                           = default;
    LogicalDevice(const LogicalDevice&)            = delete;
    LogicalDevice(LogicalDevice&&)                 = delete;
    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&)      = delete;

    bool create(const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family);
    void destroy(void);

    // Retrieval
    const PhysicalDevice& get_physical_device(void) const;
    VkDevice              get_handle(void) const;
    VkQueue               get_queue(QueueType type) const;
    const QueueFamily*    get_queue_family(QueueType type) const;

    // Commands
    bool                         queue_submit(VkCommandBuffer* command_buffers, uint32_t command_buffers_count, QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Semaphore*>& signal_sems, Fence* fence);
    uint32_t                     find_memory_type(uint32_t desired_type, VkMemoryPropertyFlags memory_properties);
    void                         wait_idle(void);
    VkResult                     wait_for_fences(std::vector<VkFence>& fences, uint64_t timeout, bool wait_all);
    void                         reset_fences(std::vector<VkFence>& fences);

    VkResult                     acquire_next_image(Swapchain* swapchain, uint64_t timeout, Semaphore* semaphore, Fence* fence, uint32_t* image_index);
    VkResult                     queue_present(QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Swapchain*>& swapchains, const std::vector<uint32_t>& image_indices, std::vector<VkResult>& results);
    VkResult                     get_swapchain_images(Swapchain* swapchain, std::vector<VkImage>& images);

    VkMemoryRequirements         get_buffer_memory_reqs(VkBuffer buffer);
    VkResult                     bind_buffer_memory(VkBuffer buffer, VkDeviceMemory memory);

    VkMemoryRequirements         get_image_memory_reqs(VkImage image);
    VkResult                     bind_image_memory(VkImage image, VkDeviceMemory memory);

    std::vector<VkDescriptorSet> allocate_descriptor_sets(VkDescriptorSetLayout* layouts, uint32_t layouts_count, VkDescriptorPool pool);
    void                         free_descriptor_sets(VkDescriptorSet* sets, uint32_t sets_count, VkDescriptorPool pool);
    void                         update_descriptor_sets(VkWriteDescriptorSet* write_sets, uint32_t write_sets_count);

    std::vector<VkCommandBuffer> allocate_command_buffers(uint32_t count, VkCommandBufferLevel level, VkCommandPool pool) const;
    void                         free_command_buffers(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool) const;

    VkDeviceMemory        allocate_memory(VkMemoryAllocateInfo& alloc_info);
    void                  free_memory(VkDeviceMemory memory);
    bool                  map_memory(VkDeviceMemory memory, size_t size_bytes, uint64_t offset_bytes, void** mapped);
    void                  unmap_memory(VkDeviceMemory memory);

    VkSwapchainKHR        create_swapchain(VkSwapchainCreateInfoKHR& create_info);
    void                  destroy_swapchain(VkSwapchainKHR swapchain);

    VkRenderPass          create_render_pass(VkRenderPassCreateInfo& create_info);
    void                  destroy_render_pass(VkRenderPass render_pass);

    VkFramebuffer         create_framebuffer(VkFramebufferCreateInfo& create_info);
    void                  destroy_framebuffer(VkFramebuffer framebuffer);

    VkDescriptorPool      create_descriptor_pool(VkDescriptorPoolCreateInfo& create_info);
    void                  destroy_descriptor_pool(VkDescriptorPool pool);

    VkDescriptorSetLayout create_descriptor_set_layout(VkDescriptorSetLayoutCreateInfo& create_info);
    void                  destroy_descriptor_set_layout(VkDescriptorSetLayout layout);

    VkCommandPool         create_command_pool(VkCommandPoolCreateInfo& create_info) const;
    void                  destroy_command_pool(VkCommandPool pool) const;

    VkImageView           create_image_view(VkImageViewCreateInfo& create_info);
    void                  destroy_image_view(VkImageView image_view);

    VkBuffer              create_buffer(VkBufferCreateInfo& create_info);
    void                  destroy_buffer(VkBuffer buffer);

    VkPipelineLayout      create_pipeline_layout(VkPipelineLayoutCreateInfo& create_info);
    void                  destroy_pipeline_layout(VkPipelineLayout layout);

    VkPipeline            create_pipeline(VkGraphicsPipelineCreateInfo& create_info);
    void                  destroy_pipeline(VkPipeline pipeline);

    VkEvent               create_event(const VkEventCreateInfo& create_info);
    void                  destroy_event(VkEvent event);

    VkFence               create_fence(const VkFenceCreateInfo& create_info);
    void                  destroy_fence(VkFence fence);

    VkSemaphore           create_semaphore(const VkSemaphoreCreateInfo& create_info);
    void                  destroy_semaphore(VkSemaphore semaphore);

    VkImage               create_image(VkImageCreateInfo& create_info);
    void                  destroy_image(VkImage image);

    VkSampler             create_sampler(VkSamplerCreateInfo& create_info);
    void                  destroy_sampler(VkSampler sampler);

    VkShaderModule        create_shader_module(VkShaderModuleCreateInfo& create_info);
    void                  destroy_shader_module(VkShaderModule module);

private:
    const PhysicalDevice* _physical_device{ nullptr };
    const QueueFamily*    _graphics_family{ nullptr };
    const QueueFamily*    _transfer_family{ nullptr };

    VkDevice _vk_device{ VK_NULL_HANDLE };
    VkQueue  _vk_graphics_queue{ VK_NULL_HANDLE };
    VkQueue  _vk_transfer_queue{ VK_NULL_HANDLE };
};

}

#endif
