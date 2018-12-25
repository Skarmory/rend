#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>

namespace rend
{

class CommandBuffer;
class CommandPool;
class LogicalDevice;

class Image
{
    friend class LogicalDevice;

public:
    Image(const Image&) = delete;
    Image(Image&&)      = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&)      = delete;

    VkImage       get_handle(void) const;
    VkImageView   get_view(void) const;
    VkImageLayout get_layout(void) const;
    VkExtent3D    get_extent(void) const;
    uint32_t      get_array_layers(void) const;
    uint32_t      get_mip_levels(void) const;

    bool loaded(void) const;

    bool load(void* data, size_t size_bytes, CommandPool* buffer);

    bool transition(VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage, VkImageLayout transition_to, CommandPool* pool);

private:
    Image(LogicalDevice* device, VkExtent3D extent, VkImageType type, VkFormat format,
          uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling,
          VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage,
          VkImageViewType view_type);
    ~Image(void);

private:
    VkImage               _vk_image;
    VkImageView           _vk_image_view;
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
