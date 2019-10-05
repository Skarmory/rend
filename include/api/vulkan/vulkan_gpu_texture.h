#ifndef REND_VULKAN_GPU_TEXTURE_H
#define REND_VULKAN_GPU_TEXTURE_H

#include "gpu_texture_base.h"
#include "rend_defs.h"

#include <vulkan.h>

namespace rend
{

VkImageType find_image_type(VkExtent3D extent);

class DeviceContext;

class VulkanGPUTexture : public GPUTextureBase
{
public:
    VulkanGPUTexture(DeviceContext& context);
    ~VulkanGPUTexture(void);
    VulkanGPUTexture(const VulkanGPUTexture&)            = delete;
    VulkanGPUTexture(VulkanGPUTexture&&)                 = delete;
    VulkanGPUTexture& operator=(const VulkanGPUTexture&) = delete;
    VulkanGPUTexture& operator=(VulkanGPUTexture&&)      = delete;


    VkImage       get_handle(void) const;
    VkImageView   get_view(void) const;
    VkSampler     get_sampler(void) const;
    VkImageLayout get_layout(void) const;
    VkExtent3D    get_extent(void) const;
    uint32_t      get_array_layers(void) const;
    uint32_t      get_mip_levels(void) const;

    VkDeviceMemory        get_memory(void) const;
    VkMemoryPropertyFlags get_memory_properties(void) const;

    void transition(VkImageLayout final_layout);

protected:
    StatusCode create_texture_api(
        VkExtent3D extent, VkImageType type, VkFormat format,
        uint32_t mip_levels, uint32_t array_layers,
        VkSampleCountFlagBits samples, VkImageTiling tiling,
        VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage,
        VkImageViewType view_type, VkImageAspectFlags aspects
    );

private:
    bool _create_vk_image(VkImageType type, VkFormat format, VkExtent3D extent, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage);
    bool _alloc_vk_memory(VkMemoryPropertyFlags memory_properties);
    bool _create_vk_image_view(VkFormat format, VkImageViewType view_type, VkImageAspectFlags aspects, uint32_t mip_levels, uint32_t array_layers);
    bool _create_vk_sampler(void);

private:
    DeviceContext& _context;
    bool           _loaded;

    VkImage               _vk_image;
    VkImageView           _vk_image_view;
    VkSampler             _vk_sampler;
    VkDeviceMemory        _vk_memory;
    VkMemoryPropertyFlags _vk_memory_properties;
    VkImageType           _vk_type;
    VkFormat              _vk_format;
    uint32_t              _mip_levels;
    uint32_t              _array_layers;
    VkSampleCountFlagBits _vk_samples;
    VkImageTiling         _vk_tiling;
    VkImageUsageFlags     _vk_usage;
    VkImageLayout         _vk_layout;
};

}

#endif
