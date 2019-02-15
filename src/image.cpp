#include "image.h"

#include <cstring>

#include "command_buffer.h"
#include "command_pool.h"
#include "fence.h"
#include "logical_device.h"
#include "utils.h"

using namespace rend;

Image::Image(LogicalDevice* device, VkExtent3D extent, VkImageType type, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspects)
    : _vk_memory_properties(memory_properties), _vk_extent(extent), _vk_type(type), _vk_format(format), _mip_levels(mip_levels), _array_layers(array_layers), _vk_samples(samples), _vk_tiling(tiling), _vk_usage(usage), _vk_layout(VK_IMAGE_LAYOUT_UNDEFINED),  _device(device), _size_bytes(0), _loaded(false)
{
    uint32_t queue_family_index = _device->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkImageCreateInfo image_info =
    {
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .imageType             = type,
        .format                = format,
        .extent                = extent,
        .mipLevels             = mip_levels,
        .arrayLayers           = array_layers,
        .samples               = samples,
        .tiling                = tiling,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices   = &queue_family_index,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VULKAN_DEATH_CHECK(vkCreateImage(_device->get_handle(), &image_info, nullptr, &_vk_image), "Failed to create image");

    VkMemoryRequirements memory_reqs;
    vkGetImageMemoryRequirements(_device->get_handle(), _vk_image, &memory_reqs);
    
    VkMemoryAllocateInfo alloc_info =
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memory_reqs.size,
        .memoryTypeIndex = _device->find_memory_type(memory_reqs.memoryTypeBits, memory_properties)
    };

    VULKAN_DEATH_CHECK(vkAllocateMemory(_device->get_handle(), &alloc_info, nullptr, &_vk_memory), "Failed to allocate memory");

    _size_bytes = memory_reqs.size;

    VULKAN_DEATH_CHECK(vkBindImageMemory(_device->get_handle(), _vk_image, _vk_memory, 0), "Failed to bind image memory");

    VkImageViewCreateInfo image_view_info =
    {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .image            = _vk_image,
        .viewType         = view_type,
        .format           = _vk_format,
        .components       =
        {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A
        },
        .subresourceRange =
        {
            .aspectMask     = aspects,
            .baseMipLevel   = 0,
            .levelCount     = _mip_levels,
            .baseArrayLayer = 0,
            .layerCount     = _array_layers
        }
    };

    VULKAN_DEATH_CHECK(vkCreateImageView(_device->get_handle(), &image_view_info, nullptr, &_vk_image_view), "Failed to create image view");

    VkSamplerCreateInfo sampler_info =
    {
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .magFilter               = VK_FILTER_LINEAR,
        .minFilter               = VK_FILTER_LINEAR,
        .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias              = 1.0f,
        .anisotropyEnable        = VK_FALSE,
        .maxAnisotropy           = 1.0f,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = 1.0f,
        .maxLod                  = 1.0f,
        .borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    VULKAN_DEATH_CHECK(vkCreateSampler(_device->get_handle(), &sampler_info, nullptr, &_vk_sampler), "Failed to create sampler");
}

Image::~Image(void)
{
    vkDestroySampler(_device->get_handle(), _vk_sampler, nullptr);
    vkDestroyImageView(_device->get_handle(), _vk_image_view, nullptr);
    vkFreeMemory(_device->get_handle(), _vk_memory, nullptr);
    vkDestroyImage(_device->get_handle(), _vk_image, nullptr);
}

VkImage Image::get_handle(void) const
{
    return _vk_image;
}

VkImageView Image::get_view(void) const
{
    return _vk_image_view;
}

VkSampler Image::get_sampler(void) const
{
    return _vk_sampler;
}

VkImageLayout Image::get_layout(void) const
{
    return _vk_layout;
}

VkExtent3D Image::get_extent(void) const
{
    return _vk_extent;
}

uint32_t Image::get_array_layers(void) const
{
    return _array_layers;
}

uint32_t Image::get_mip_levels(void) const
{
    return _mip_levels;
}

VkDeviceMemory Image::get_memory(void) const
{
    return _vk_memory;
}

VkMemoryPropertyFlags Image::get_memory_properties(void) const
{
    return _vk_memory_properties;
}
