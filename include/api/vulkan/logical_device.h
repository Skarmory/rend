#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H

#include "queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandBuffer;
class CommandPool;
class DescriptorPool;
class DescriptorSetLayout;
class DeviceContext;
class Event;
class Fence;
class Framebuffer;
class GPUBuffer;
class Image;
class PhysicalDevice;
class Pipeline;
class PipelineLayout;
class RenderPass;
class Semaphore;
class Shader;
class Swapchain;

struct PipelineSettings;

enum class ShaderType;

class LogicalDevice
{

public:
    LogicalDevice(const DeviceContext* device_context, const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family);
    ~LogicalDevice(void);

    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&&) = delete;

    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

    bool create_logical_device(void);

    // Retrieval
    const DeviceContext&  get_device_context(void) const;
    const PhysicalDevice& get_physical_device(void) const;
    VkDevice              get_handle(void) const;
    VkQueue               get_queue(QueueType type) const;
    const QueueFamily*    get_queue_family(QueueType type) const;

    // Commands
    bool                  queue_submit(const std::vector<CommandBuffer*>& command_buffers, QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Semaphore*>& signal_sems, Fence* fence);
    uint32_t              find_memory_type(uint32_t desired_type, VkMemoryPropertyFlags memory_properties);

    // Creational
    Swapchain*            create_swapchain(uint32_t desired_images);
    void                  destroy_swapchain(Swapchain** swapchain);

    RenderPass*           create_render_pass(const std::vector<VkAttachmentDescription>& attachment_descs, const std::vector<VkSubpassDescription>& subpass_descs, const std::vector<VkSubpassDependency>& subpass_deps);
    void                  destroy_render_pass(RenderPass** render_pass);

    Framebuffer*          create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& image_views, VkExtent3D dimensions);
    void                  destroy_framebuffer(Framebuffer** framebuffer);

    DescriptorPool*       create_descriptor_pool(uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& pool_sizes);
    void                  destroy_descriptor_pool(DescriptorPool** pool);

    DescriptorSetLayout*  create_descriptor_set_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    void                  destroy_descriptor_set_layout(DescriptorSetLayout** layout);

    PipelineLayout*       create_pipeline_layout(const std::vector<DescriptorSetLayout*>& desc_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges);
    void                  destroy_pipeline_layout(PipelineLayout** layout);

    Pipeline*             create_pipeline(PipelineSettings* settings);
    void                  destroy_pipeline(Pipeline** pipeline);

    Shader*               create_shader(const void* data, uint32_t size_bytes, ShaderType type);
    void                  destroy_shader(Shader** shader);

    Semaphore*            create_semaphore(void);
    void                  destroy_semaphore(Semaphore** semaphore);

    Fence*                create_fence(bool start_signalled=false);
    void                  destroy_fence(Fence** fence);

    Event*                create_event(void);
    void                  destroy_event(Event** event);

    Image*                create_image(VkExtent3D extent, VkImageType type, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspects);
    void                  destroy_image(Image** image);

private:
    VkDevice _vk_device;
    VkQueue  _vk_graphics_queue;
    VkQueue  _vk_transfer_queue;

    const DeviceContext*  _context;
    const PhysicalDevice* _physical_device;
    const QueueFamily*    _graphics_family;
    const QueueFamily*    _transfer_family;

    std::vector<CommandPool*> _command_pools;
};

}

#endif
