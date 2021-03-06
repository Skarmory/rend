#ifndef REND_VULKAN_DEVICE_CONTEXT_H
#define REND_VULKAN_DEVICE_CONTEXT_H

#include "data_array.h"
#include "device_context.h"
#include "rend_defs.h"
#include "resource.h"

#include <unordered_map>
#include <vulkan.h>
#include <vector>

namespace rend::vkal::memory
{
	class GPUMemoryInterface;
}

namespace rend
{

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
    //rend::vkal::memory::GPUMemoryInterface* memory_interface(void) const;
    LogicalDevice*                          get_device(void) const;

    StatusCode create(void) override;
    void       destroy(void) override;

    StatusCode choose_gpu(const VkPhysicalDeviceFeatures& desired_features);
    StatusCode create_device(const VkQueueFlags desired_queues);

    [[nodiscard]] VertexBufferHandle  create_vertex_buffer(uint32_t vertices_count, size_t vertex_size) override;
    [[nodiscard]] IndexBufferHandle   create_index_buffer(uint32_t indices_count, size_t index_size) override;
    [[nodiscard]] UniformBufferHandle create_uniform_buffer(size_t bytes);
    [[nodiscard]] TextureHandle       create_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mips, uint32_t layers, Format format, MSAASamples samples, ImageUsage usage) override;
    [[nodiscard]] ShaderHandle        create_shader(const ShaderType type, const void* code, const size_t bytes) override;
    [[nodiscard]] FramebufferHandle   create_framebuffer(const FramebufferInfo& info) override;
    [[nodiscard]] RenderPassHandle    create_render_pass(const RenderPassInfo& info) override;
    [[nodiscard]] Texture2DHandle     register_swapchain_image(VkImage swapchain_image, VkFormat format);

    void destroy_buffer(BufferHandle handle) override;
    void destroy_texture(TextureHandle handle) override;
    void destroy_image_view(TextureHandle handle);
    void destroy_shader(ShaderHandle handle) override;
    void destroy_framebuffer(FramebufferHandle handle) override;
    void destroy_render_pass(RenderPassHandle handle) override;
    void unregister_swapchain_image(TextureHandle swapchain_handle);

    VkBuffer       get_buffer(VertexBufferHandle handle) const;
    VkImage        get_image(TextureHandle handle) const;
    VkImageView    get_image_view(TextureHandle handle) const;
    VkSampler      get_sampler(TextureHandle handle) const;
    VkDeviceMemory get_memory(HandleType handle) const;
    VkShaderModule get_shader(const ShaderHandle handle) const;
    VkFramebuffer  get_framebuffer(const FramebufferHandle handle) const;
    VkRenderPass   get_render_pass(const RenderPassHandle handle) const;

private:
    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

    BufferHandle _create_buffer_internal(size_t bytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties);
    VkImageView  _create_image_view(VkImage image, VkFormat format, VkImageViewType type, VkImageAspectFlags aspect, uint32_t mips, uint32_t layers);
    VkSampler    _create_sampler(void);

    void _destroy_sampler(TextureHandle handle);

private:
    std::vector<PhysicalDevice*>             _physical_devices;
    //rend::vkal::memory::GPUMemoryInterface*  _memory_interface  { nullptr };
    LogicalDevice*                           _logical_device    { nullptr };
    PhysicalDevice*                          _chosen_gpu        { nullptr };
    uint16_t _data_array_unique_key{ 1 };

    DataArray<VkBuffer>       _vk_buffers;
    DataArray<VkImage>        _vk_images;
    DataArray<VkImageView>    _vk_image_views;
    DataArray<VkSampler>      _vk_samplers;
    DataArray<VkDeviceMemory> _vk_memorys;
    DataArray<VkShaderModule> _vk_shaders;
    DataArray<VkFramebuffer>  _vk_framebuffers;
    DataArray<VkRenderPass>   _vk_render_passes;

    std::unordered_map<HandleType, MemoryHandle>           _handle_to_memory_handle;
    std::unordered_map<Texture2DHandle, TextureViewHandle> _texture_handle_to_view_handle;
    std::unordered_map<Texture2DHandle, SamplerHandle>     _texture_handle_to_sampler_handle;
};

}

#endif
