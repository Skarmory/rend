#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>

namespace rend
{

class CommandPool;
class LogicalDevice;

class Image
{
    friend class LogicalDevice;
    friend class ImageTransitionTask;

public:
    Image(const Image&) = delete;
    Image(Image&&)      = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&)      = delete;

    VkImage       get_handle(void) const;
    VkImageView   get_view(void) const;
    VkSampler     get_sampler(void) const;
    VkImageLayout get_layout(void) const;
    VkExtent3D    get_extent(void) const;
    uint32_t      get_array_layers(void) const;
    uint32_t      get_mip_levels(void) const;

    VkDeviceMemory        get_memory(void) const;
    VkMemoryPropertyFlags get_memory_properties(void) const;

private:
    Image(LogicalDevice* device, VkExtent3D extent, VkImageType type, VkFormat format,
          uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling,
          VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage,
          VkImageViewType view_type);
    ~Image(void);

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

    LogicalDevice* _device;
    size_t         _size_bytes;
    bool           _loaded;
};

}

#endif
