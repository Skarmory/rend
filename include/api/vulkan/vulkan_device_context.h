#ifndef REND_VULKAN_DEVICE_CONTEXT_H
#define REND_VULKAN_DEVICE_CONTEXT_H

#include "core/device_context.h"
#include "core/rend_defs.h"
#include "core/data_structures/data_array.h"
//#include "resource.h"

#include <unordered_map>
#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandPool;
class PhysicalDevice;
class LogicalDevice;

class VulkanDeviceContext : public DeviceContext
{
public:
    VulkanDeviceContext(void);
    VulkanDeviceContext(const VulkanDeviceContext&)            = delete;
    VulkanDeviceContext(VulkanDeviceContext&&)                 = delete;
    VulkanDeviceContext& operator=(const VulkanDeviceContext&) = delete;
    VulkanDeviceContext& operator=(VulkanDeviceContext&&)      = delete;

    PhysicalDevice*                         gpu(void) const;
    LogicalDevice*                          get_device(void) const;

    StatusCode create(void) override;
    void       destroy(void) override;

    StatusCode choose_gpu(const VkPhysicalDeviceFeatures& desired_features);
    StatusCode create_device(const VkQueueFlags desired_queues);

    [[nodiscard]] VertexBufferHandle        create_vertex_buffer(uint32_t vertices_count, size_t vertex_size) override;
    [[nodiscard]] IndexBufferHandle         create_index_buffer(uint32_t indices_count, size_t index_size) override;
    [[nodiscard]] UniformBufferHandle       create_uniform_buffer(size_t bytes);
    [[nodiscard]] TextureHandle             create_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mips, uint32_t layers, Format format, MSAASamples samples, ImageUsage usage) override;
    [[nodiscard]] ShaderHandle              create_shader(const ShaderStage type, const void* code, const size_t bytes) override;
    [[nodiscard]] FramebufferHandle         create_framebuffer(const FramebufferInfo& info) override;
    [[nodiscard]] RenderPassHandle          create_render_pass(const RenderPassInfo& info) override;
    [[nodiscard]] PipelineLayoutHandle      create_pipeline_layout(const PipelineLayoutInfo& info) override;
    [[nodiscard]] PipelineHandle            create_pipeline(const PipelineInfo& info) override;
    [[nodiscard]] CommandPoolHandle         create_command_pool(void) override;
    [[nodiscard]] CommandBufferHandle       create_command_buffer(CommandPoolHandle pool_handle) override;
    [[nodiscard]] DescriptorPoolHandle      create_descriptor_pool(const DescriptorPoolInfo& info) override;
    [[nodiscard]] DescriptorSetLayoutHandle create_descriptor_set_layout(const DescriptorSetLayoutInfo& info) override;
    [[nodiscard]] DescriptorSetHandle       create_descriptor_set(const DescriptorSetInfo& info) override;
    [[nodiscard]] Texture2DHandle           register_swapchain_image(VkImage swapchain_image, VkFormat format);

    void destroy_buffer(BufferHandle handle) override;
    void destroy_texture(TextureHandle handle) override;
    void destroy_image_view(TextureHandle handle);
    void destroy_shader(ShaderHandle handle) override;
    void destroy_framebuffer(FramebufferHandle handle) override;
    void destroy_render_pass(RenderPassHandle handle) override;
    void destroy_pipeline_layout(PipelineLayoutHandle handle) override;
    void destroy_pipeline(PipelineHandle handle) override;
    void destroy_command_buffer(CommandBufferHandle handle) override;
    void destroy_descriptor_pool(DescriptorPoolHandle handle) override;
    void destroy_descriptor_set_layout(DescriptorSetLayoutHandle handle) override;
    void destroy_descriptor_set(DescriptorSetHandle handle) override;
    void unregister_swapchain_image(TextureHandle swapchain_handle);

    // Command Buffer
    void bind_descriptor_sets(CommandBufferHandle command_buffer_handle, PipelineBindPoint bind_point, PipelineHandle pipeline_handle, DescriptorSet* descriptor_set, size_t descriptor_set_count) override;
    void bind_pipeline(CommandBufferHandle cmd_buffer, PipelineBindPoint bind_point, PipelineHandle handle) override;
    void bind_vertex_buffer(CommandBufferHandle command_buffer_handle, BufferHandle handle) override;
    void bind_index_buffer(CommandBufferHandle command_buffer_handle, BufferHandle handle) override;
    void command_buffer_begin(CommandBufferHandle command_buffer_handle) override;
    void command_buffer_end(CommandBufferHandle command_buffer_handle) override;
    void command_buffer_reset(CommandBufferHandle command_buffer_handle) override;
    void copy_buffer_to_buffer(CommandBufferHandle command_buffer_handle, BufferHandle src_handle, BufferHandle dst_handle, const BufferBufferCopyInfo& info) override;
    void copy_buffer_to_image(CommandBufferHandle command_buffer_handle, BufferHandle src_buffer_handle, TextureHandle dst_texture_handle, const BufferImageCopyInfo& info) override;
    void draw(CommandBufferHandle command_buffer_handle, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) override;
    void pipeline_barrier(const CommandBufferHandle command_buffer_handle, const PipelineBarrierInfo& info) override;
    void push_constant(const CommandBufferHandle command_buffer_handle, const PipelineLayoutHandle layout_handle, ShaderStages stages, uint32_t offset, size_t size, const void* data) override;
    void set_viewport(const CommandBufferHandle command_buffer_handle, const ViewportInfo* infos, size_t infos_count) override;
    void set_scissor(const CommandBufferHandle command_buffer_handle, const ViewportInfo* infos, size_t infos_count) override;
    void begin_render_pass(const CommandBufferHandle command_buffer_handle, const RenderPassHandle render_pass_handle, const FramebufferHandle framebuffer_handle, const RenderArea render_area, const ColourClear clear_colour, const DepthStencilClear clear_depth_stencil );
    void end_render_pass(const CommandBufferHandle command_buffer_handle);

    void add_descriptor_binding(const DescriptorSetHandle handle, const DescriptorSetBinding& binding) override;

    VkBuffer         get_buffer(VertexBufferHandle handle) const;
    VkImage          get_image(TextureHandle handle) const;
    VkImageView      get_image_view(TextureHandle handle) const;
    VkSampler        get_sampler(TextureHandle handle) const;
    VkDeviceMemory   get_memory(HandleType handle) const;
    VkShaderModule   get_shader(const ShaderHandle handle) const;
    VkFramebuffer    get_framebuffer(const FramebufferHandle handle) const;
    VkRenderPass     get_render_pass(const RenderPassHandle handle) const;
    VkCommandBuffer  get_command_buffer(const CommandBufferHandle handle) const;
    VkPipelineLayout get_pipeline_layout(const PipelineLayoutHandle handle) const;
    VkDescriptorSetLayout get_descriptor_set_layout(const DescriptorSetLayoutHandle handle) const;
    VkDescriptorSet get_descriptor_set(const DescriptorSetHandle handle) const;
    VkDescriptorPool get_descriptor_pool(const DescriptorPoolHandle handle) const;

private:
    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

    BufferHandle _create_buffer_internal(size_t bytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties);
    VkImageView  _create_image_view(VkImage image, VkFormat format, VkImageViewType type, VkImageAspectFlags aspect, uint32_t mips, uint32_t layers);
    VkSampler    _create_sampler(void);

    void _destroy_sampler(TextureHandle handle);

private:
    std::vector<PhysicalDevice*> _physical_devices;
    LogicalDevice*               _logical_device    { nullptr };
    PhysicalDevice*              _chosen_gpu        { nullptr };
    uint16_t _data_array_unique_key{ 1 };

    DataArray<VkBuffer>              _vk_buffers;
    DataArray<VkImage>               _vk_images;
    DataArray<VkImageView>           _vk_image_views;
    DataArray<VkSampler>             _vk_samplers;
    DataArray<VkDeviceMemory>        _vk_memorys;
    DataArray<VkShaderModule>        _vk_shaders;
    DataArray<VkFramebuffer>         _vk_framebuffers;
    DataArray<VkRenderPass>          _vk_render_passes;
    DataArray<VkPipeline>            _vk_pipelines;
    DataArray<VkPipelineLayout>      _vk_pipeline_layouts;
    DataArray<VkDescriptorSetLayout> _vk_descriptor_set_layouts;
    DataArray<VkDescriptorSet>       _vk_descriptor_sets;
    DataArray<VkDescriptorPool>      _vk_descriptor_pools;

    DataArray<CommandPool> _command_pools;

    std::unordered_map<HandleType, MemoryHandle>           _handle_to_memory_handle;
    std::unordered_map<Texture2DHandle, TextureViewHandle> _texture_handle_to_view_handle;
    std::unordered_map<Texture2DHandle, SamplerHandle>     _texture_handle_to_sampler_handle;
    std::unordered_map<CommandBufferHandle, HandleType>    _buffer_handle_to_pool_handle;
    std::unordered_map<DescriptorSetHandle, DescriptorPoolHandle> _descriptor_set_handle_to_descriptor_pool_handle;
};

}

#endif
