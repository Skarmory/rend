#ifndef REND_VULKAN_DEVICE_CONTEXT_H
#define REND_VULKAN_DEVICE_CONTEXT_H

#include "core/data_structures/data_array.h"
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
    ~VulkanDeviceContext(void);
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
    [[nodiscard]] Texture2DHandle     create_texture_2d(uint32_t width, uint32_t height, uint32_t mips, uint32_t layers, Format format, ImageUsage usage) override;
    [[nodiscard]] Texture2DHandle     register_swapchain_image(VkImage swapchain_image, VkFormat format);

    void destroy_buffer(BufferHandle handle) override;
    void destroy_texture(Texture2DHandle handle) override;
    void destroy_image_view(Texture2DHandle handle);
    void unregister_swapchain_image(Texture2DHandle swapchain_handle);

    VkBuffer       get_buffer(VertexBufferHandle handle) const;
    VkImage        get_image(Texture2DHandle handle) const;
    VkImageView    get_image_view(Texture2DHandle handle) const;
    VkSampler      get_sampler(Texture2DHandle handle) const;
    VkDeviceMemory get_memory(HandleType handle) const;

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

    DataArray<VkBuffer> _vk_buffers;
    DataArray<VkImage> _vk_images;
    DataArray<VkImageView> _vk_image_views;
    DataArray<VkSampler> _vk_samplers;
    DataArray<VkDeviceMemory> _vk_memorys;
    std::unordered_map<HandleType, MemoryHandle> _handle_to_memory_handle;
    std::unordered_map<Texture2DHandle, TextureViewHandle> _texture_handle_to_view_handle;
    std::unordered_map<Texture2DHandle, SamplerHandle> _texture_handle_to_sampler_handle;
};

}

#endif
