#ifndef REND_VULKAN_DEVICE_CONTEXT_H
#define REND_VULKAN_DEVICE_CONTEXT_H

#include "core/rend_defs.h"
#include "core/containers/data_array.h"
#include "api/vulkan/vulkan_buffer_info.h"
#include "api/vulkan/vulkan_descriptor_set_info.h"
#include "api/vulkan/vulkan_image_info.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class DescriptorSet;
class GPUBuffer;
class GPUTexture;
class LogicalDevice;
class PhysicalDevice;
class VulkanInstance;
class Window;
struct BufferInfo;
struct DescriptorSetBinding;
struct DescriptorSetLayoutInfo;
struct FramebufferInfo;
struct PipelineInfo;
struct PipelineLayoutInfo;
struct RenderPassInfo;
struct TextureInfo;
struct VulkanInitInfo;

class VulkanDeviceContext
{
public:
    VulkanDeviceContext(const VulkanInitInfo& rend_info, const Window& window);
    ~VulkanDeviceContext(void);
    VulkanDeviceContext(const VulkanDeviceContext&)            = delete;
    VulkanDeviceContext(VulkanDeviceContext&&)                 = delete;
    VulkanDeviceContext& operator=(const VulkanDeviceContext&) = delete;
    VulkanDeviceContext& operator=(VulkanDeviceContext&&)      = delete;

    PhysicalDevice*                     gpu(void) const;
    LogicalDevice*                      get_device(void) const;
    const VulkanInstance&               vulkan_instance(void) const;

    [[nodiscard]] VulkanBufferInfo        create_buffer(const BufferInfo& info, VkMemoryPropertyFlags memory_properties);
    [[nodiscard]] VkCommandBuffer         create_command_buffer(VkCommandPool command_pool);
    [[nodiscard]] VkCommandPool           create_command_pool(void);
    [[nodiscard]] VkDescriptorPool        create_descriptor_pool(const DescriptorPoolInfo& info);
    [[nodiscard]] VulkanDescriptorSetInfo create_descriptor_set(VkDescriptorPool pool, VkDescriptorSetLayout layout);
    [[nodiscard]] VkDescriptorSetLayout   create_descriptor_set_layout(const DescriptorSetLayoutInfo& info);
    [[nodiscard]] VkEvent                 create_event(const VkEventCreateInfo& info);
    [[nodiscard]] VkFence                 create_fence(const VkFenceCreateInfo& info);
    [[nodiscard]] VkFramebuffer           create_framebuffer(const FramebufferInfo& info);
    [[nodiscard]] VkPipeline              create_pipeline(const PipelineInfo& info);
    [[nodiscard]] VkPipelineLayout        create_pipeline_layout(const PipelineLayoutInfo& info);
    [[nodiscard]] VkRenderPass            create_render_pass(const RenderPassInfo& info);
    [[nodiscard]] VkSemaphore             create_semaphore(const VkSemaphoreCreateInfo& info);
    [[nodiscard]] VkShaderModule          create_shader(const void* code, const size_t bytes);
    [[nodiscard]] VulkanImageInfo         create_texture(const TextureInfo& info);
    [[nodiscard]] VulkanImageInfo         register_swapchain_image(VkImage swapchain_image, VkFormat format);

    void destroy_buffer(const VulkanBufferInfo& buffer_info);
    void destroy_command_buffer(VkCommandBuffer command_buffer, VkCommandPool pool);
    void destroy_command_pool(VkCommandPool command_pool);
    void destroy_descriptor_pool(VkDescriptorPool pool);
    void destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout);
    void destroy_descriptor_set(const VulkanDescriptorSetInfo& set_info);
    void destroy_event(VkEvent event);
    void destroy_fence(VkFence fence);
    void destroy_framebuffer(VkFramebuffer framebuffer);
    void destroy_pipeline(VkPipeline pipeline);
    void destroy_pipeline_layout(VkPipelineLayout layout);
    void destroy_render_pass(VkRenderPass render_pass);
    void destroy_texture(const VulkanImageInfo& image_info);
    void destroy_semaphore(VkSemaphore semaphore);
    void destroy_shader(VkShaderModule shader);
    void unregister_swapchain_image(const VulkanImageInfo& image_info);

    void  write_descriptor_bindings(VkDescriptorSet descriptor_set, const std::vector<DescriptorSetBinding>& binding);
    void* map_buffer_memory(GPUBuffer& buffer, size_t bytes);
    void  unmap_buffer_memory(GPUBuffer& buffer);
    void* map_image_memory(GPUTexture& texture, size_t bytes);
    void  unmap_image_memory(GPUTexture& texture);

    void set_debug_name(const char* name, VkObjectType type, uint64_t handle);

private:
    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

    VkImageView  _create_image_view(VkImage image, VkFormat format, VkImageViewType type, VkImageAspectFlags aspect, uint32_t mips, uint32_t layers);
    VkSampler    _create_sampler(void);

private:
    std::vector<PhysicalDevice*> _physical_devices;
    VulkanInstance*              _vulkan_instance{ nullptr };
    LogicalDevice*               _logical_device{ nullptr };
    PhysicalDevice*              _chosen_gpu{ nullptr };
};

}

#endif
