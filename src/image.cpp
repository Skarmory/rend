#include "image.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

Image::Image(LogicalDevice* device, VkExtent3D extent, VkImageType type, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage, VkImageViewType view_type)
    : _vk_extent(extent), _vk_type(type), _vk_format(format), _mip_levels(mip_levels), _array_layers(array_layers), _vk_samples(samples), _vk_tiling(tiling), _vk_usage(usage), _device(device)
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
        .samples              = samples,
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
    
    VkDeviceSize size_bytes = _vk_extent.width * _vk_extent.height * _vk_extent.depth * 4;
    VkMemoryAllocateInfo alloc_info =
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = size_bytes,
        .memoryTypeIndex = _device->find_memory_type(memory_reqs.memoryTypeBits, memory_properties)
    };

    VULKAN_DEATH_CHECK(vkAllocateMemory(_device->get_handle(), &alloc_info, nullptr, &_vk_memory), "Failed to allocate memory");

    VULKAN_DEATH_CHECK(vkBindImageMemory(_device->get_handle(), _vk_image, _vk_memory, 0), "Failed to bind image memory");

    VkComponentMapping mapping;
    VkImageSubresourceRange subresource_range =
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = _mip_levels,
        .baseArrayLayer = 0,
        .layerCount     = _array_layers
    };

    VkImageViewCreateInfo image_view_info =
    {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .image            = _vk_image,
        .viewType         = view_type,
        .format           = _vk_format,
        .components       = mapping,
        .subresourceRange = subresource_range
    };

    VULKAN_DEATH_CHECK(vkCreateImageView(_device->get_handle(), &image_view_info, nullptr, &_vk_image_view), "Failed to create image view");
}

Image::~Image(void)
{
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
