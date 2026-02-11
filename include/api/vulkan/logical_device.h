#ifndef REND_API_VULKAN_LOGICAL_DEVICE_H
#define REND_API_VULKAN_LOGICAL_DEVICE_H

#include "api/vulkan/device_features.h"
#include "api/vulkan/extension_funcs.h"
#include "api/vulkan/queue_family.h"

#include <cstdint>
#include <string>
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
    LogicalDevice(const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family, const std::vector<DeviceFeature>& features);
    ~LogicalDevice(void);
    LogicalDevice(const LogicalDevice&)            = delete;
    LogicalDevice(LogicalDevice&&)                 = delete;
    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&)      = delete;

    // Retrieval
    const PhysicalDevice& get_physical_device(void) const;
    VkDevice              get_handle(void) const;
    VkQueue               get_queue(QueueType type) const;
    const QueueFamily*    get_queue_family(QueueType type) const;

    // Commands
    bool                         queue_submit(VkCommandBuffer* command_buffers, uint32_t command_buffers_count, QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Semaphore*>& signal_sems, const Fence* fence) const;
    uint32_t                     find_memory_type(uint32_t desired_type, VkMemoryPropertyFlags memory_properties);
    void                         wait_idle(void) const;
    VkResult                     wait_for_fences(std::vector<VkFence>& fences, uint64_t timeout, bool wait_all) const;
    void                         reset_fences(std::vector<VkFence>& fences) const;

    VkResult                     acquire_next_image(Swapchain* swapchain, uint64_t timeout, Semaphore* semaphore, Fence* fence, uint32_t* image_index) const;
    VkResult                     queue_present(QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Swapchain*>& swapchains, const std::vector<uint32_t>& image_indices, std::vector<VkResult>& results) const;
    VkResult                     get_swapchain_images(VkSwapchainKHR swapchain, std::vector<VkImage>& images) const;

    VkMemoryRequirements         get_buffer_memory_reqs(VkBuffer buffer) const;
    VkResult                     bind_buffer_memory(VkBuffer buffer, VkDeviceMemory memory) const;

    VkMemoryRequirements         get_image_memory_reqs(VkImage image) const;
    VkResult                     bind_image_memory(VkImage image, VkDeviceMemory memory) const;

    std::vector<VkDescriptorSet> allocate_descriptor_sets(std::vector<VkDescriptorSetLayout>& layouts, VkDescriptorPool pool) const;
    void                         free_descriptor_sets(const VkDescriptorSet* sets, uint32_t sets_count, VkDescriptorPool pool) const;
    void                         update_descriptor_sets(std::vector<VkWriteDescriptorSet>& write_sets) const;

    std::vector<VkCommandBuffer> allocate_command_buffers(uint32_t count, VkCommandBufferLevel level, VkCommandPool pool) const;
    void                         free_command_buffers(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool) const;

    VkDeviceMemory        allocate_memory(VkMemoryAllocateInfo& alloc_info) const;
    void                  free_memory(VkDeviceMemory memory) const;
    bool                  map_memory(VkDeviceMemory memory, size_t size_bytes, uint64_t offset_bytes, void** mapped) const;
    void                  unmap_memory(VkDeviceMemory memory) const;

    VkSwapchainKHR        create_swapchain(VkSwapchainCreateInfoKHR& create_info) const;
    void                  destroy_swapchain(VkSwapchainKHR swapchain) const;

    VkRenderPass          create_render_pass(VkRenderPassCreateInfo& create_info) const;
    void                  destroy_render_pass(VkRenderPass render_pass) const;

    VkFramebuffer         create_framebuffer(VkFramebufferCreateInfo& create_info) const;
    void                  destroy_framebuffer(VkFramebuffer framebuffer) const;

    VkDescriptorPool      create_descriptor_pool(VkDescriptorPoolCreateInfo& create_info) const;
    void                  destroy_descriptor_pool(VkDescriptorPool pool) const;

    VkDescriptorSetLayout create_descriptor_set_layout(VkDescriptorSetLayoutCreateInfo& create_info) const;
    void                  destroy_descriptor_set_layout(VkDescriptorSetLayout layout) const;

    VkCommandPool         create_command_pool(VkCommandPoolCreateInfo& create_info) const;
    void                  destroy_command_pool(VkCommandPool pool) const;
    void                  reset_command_pool(VkCommandPool pool) const;

    VkImageView           create_image_view(VkImageViewCreateInfo& create_info) const;
    void                  destroy_image_view(VkImageView image_view) const;

    VkBuffer              create_buffer(VkBufferCreateInfo& create_info) const;
    void                  destroy_buffer(VkBuffer buffer) const;

    VkPipelineLayout      create_pipeline_layout(VkPipelineLayoutCreateInfo& create_info) const;
    void                  destroy_pipeline_layout(VkPipelineLayout layout) const;

    VkPipeline            create_pipeline(VkGraphicsPipelineCreateInfo& create_info) const;
    void                  destroy_pipeline(VkPipeline pipeline) const;

    VkEvent               create_event(const VkEventCreateInfo& create_info) const;
    void                  destroy_event(VkEvent event) const;

    VkFence               create_fence(const VkFenceCreateInfo& create_info) const;
    void                  destroy_fence(VkFence fence) const;

    VkSemaphore           create_semaphore(const VkSemaphoreCreateInfo& create_info) const;
    void                  destroy_semaphore(VkSemaphore semaphore) const;

    VkImage               create_image(VkImageCreateInfo& create_info) const;
    void                  destroy_image(VkImage image) const;

    VkSampler             create_sampler(VkSamplerCreateInfo& create_info) const;
    void                  destroy_sampler(VkSampler sampler) const;

    VkShaderModule        create_shader_module(VkShaderModuleCreateInfo& create_info) const;
    void                  destroy_shader_module(VkShaderModule module) const;

    void set_debug_name(const std::string& name, VkObjectType type, uint64_t handle) const;

private:
    const PhysicalDevice* _physical_device{ nullptr };
    const QueueFamily*    _graphics_family{ nullptr };
    const QueueFamily*    _transfer_family{ nullptr };

    VkDevice _vk_device{ VK_NULL_HANDLE };
    VkQueue  _vk_graphics_queue{ VK_NULL_HANDLE };
    VkQueue  _vk_transfer_queue{ VK_NULL_HANDLE };

    DeviceExtensionFuncs _ext_funcs{};
};

}

#endif
