#ifndef REND_VULKAN_GPU_TEXTURE_H
#define REND_VULKAN_GPU_TEXTURE_H

#include "gpu_texture_base.h"
#include "rend_defs.h"
#include "swapchain.h"

#include <vulkan.h>

namespace rend
{

class VulkanGPUTexture : public GPUTextureBase
{
    friend class Swapchain;

public:
    VulkanGPUTexture(void) = default;
    ~VulkanGPUTexture(void);

    VulkanGPUTexture(const VulkanGPUTexture&)            = delete;
    VulkanGPUTexture(VulkanGPUTexture&&)                 = delete;
    VulkanGPUTexture& operator=(const VulkanGPUTexture&) = delete;
    VulkanGPUTexture& operator=(VulkanGPUTexture&&)      = delete;

    VkImage               get_handle(void) const;
    VkImageView           get_view(void) const;
    VkSampler             get_sampler(void) const;
    VkImageLayout         get_layout(void) const;
    VkExtent3D            get_extent(void) const;
    VkFormat              get_vk_format(void) const;
    uint32_t              get_array_layers(void) const;
    uint32_t              get_mip_levels(void) const;
    VkSampleCountFlagBits get_sample_count(void) const;

    VkDeviceMemory        get_memory(void) const;
    VkMemoryPropertyFlags get_memory_properties(void) const;

    void transition(VkImageLayout final_layout);

protected:
    StatusCode create_texture_api(
        VkExtent3D extent, VkImageType type, VkFormat format,
        uint32_t mip_levels, uint32_t array_layers,
        VkSampleCountFlagBits sample_count, VkImageTiling tiling,
        VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage,
        VkImageViewType view_type, VkImageAspectFlags aspects
    );

    void destroy_texture_api(void);

private:
    bool _create_vk_image(VkImageType type, VkFormat format, VkExtent3D extent, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage);
    bool _alloc_vk_memory(VkMemoryPropertyFlags memory_properties);
    bool _create_vk_image_view(VkFormat format, VkImageViewType view_type, VkImageAspectFlags aspects, uint32_t mip_levels, uint32_t array_layers);
    bool _create_vk_sampler(void);

private:
    bool                  _loaded { false };  //TODO: Not necessary due to Resource base?
    bool                  _swapchain_image { false };
    VkImage               _vk_image { VK_NULL_HANDLE };
    VkImageView           _vk_image_view { VK_NULL_HANDLE };
    VkSampler             _vk_sampler { VK_NULL_HANDLE };
    VkDeviceMemory        _vk_memory { VK_NULL_HANDLE };
    VkMemoryPropertyFlags _vk_memory_properties { 0 };
    VkImageType           _vk_type { VK_IMAGE_TYPE_MAX_ENUM };
    VkFormat              _vk_format { VK_FORMAT_MAX_ENUM };
    uint32_t              _mip_levels { 0 };
    uint32_t              _array_layers { 0 };
    VkSampleCountFlagBits _vk_sample_count { VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM };
    VkImageTiling         _vk_tiling { VK_IMAGE_TILING_MAX_ENUM };
    VkImageUsageFlags     _vk_usage { 0 };
    VkImageLayout         _vk_layout { VK_IMAGE_LAYOUT_MAX_ENUM };
};

}

#endif
