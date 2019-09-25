#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan.h>

namespace rend
{

class CommandPool;
class DeviceContext;

class Image
{
public:
    Image(DeviceContext* context);
    ~Image(void);
    Image(const Image&)            = delete;
    Image(Image&&)                 = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&)      = delete;

    bool create_image(
        VkExtent3D extent, VkImageType type, VkFormat format,
        uint32_t mip_levels, uint32_t array_layers,
        VkSampleCountFlagBits samples, VkImageTiling tiling,
        VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage,
        VkImageViewType view_type, VkImageAspectFlags aspects
    );

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

private:
    bool _create_vk_image(VkImageType type, VkFormat format, VkExtent3D extent, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage);
    bool _alloc_vk_memory(VkMemoryPropertyFlags memory_properties);
    bool _create_vk_image_view(VkFormat format, VkImageViewType view_type, VkImageAspectFlags aspects, uint32_t mip_levels, uint32_t array_layers);
    bool _create_vk_sampler(void);

private:
    VkImage               _vk_image;
    VkImageView           _vk_image_view;
    VkSampler             _vk_sampler;
    VkDeviceMemory        _vk_memory;
    VkMemoryPropertyFlags _vk_memory_properties;
    VkExtent3D            _vk_extent;
    VkImageType           _vk_type;
    VkFormat              _vk_format;
    uint32_t              _mip_levels;
    uint32_t              _array_layers;
    VkSampleCountFlagBits _vk_samples;
    VkImageTiling         _vk_tiling;
    VkImageUsageFlags     _vk_usage;
    VkImageLayout         _vk_layout;

    DeviceContext* _context;
    size_t         _size_bytes;
    bool           _loaded;
};

}

#endif
