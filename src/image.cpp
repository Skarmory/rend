#include "image.h"

#include <cstring>

#include "command_buffer.h"
#include "buffer.h"
#include "logical_device.h"
#include "utils.h"

using namespace rend;

Image::Image(LogicalDevice* device, VkExtent3D extent, VkImageType type, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage, VkImageViewType view_type)
    : _vk_memory_properties(memory_properties), _vk_extent(extent), _vk_type(type), _vk_format(format), _mip_levels(mip_levels), _array_layers(array_layers), _vk_samples(samples), _vk_tiling(tiling), _vk_usage(usage), _vk_layout(VK_IMAGE_LAYOUT_UNDEFINED),  _device(device), _size_bytes(0)
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
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = _mip_levels,
            .baseArrayLayer = 0,
            .layerCount     = _array_layers
        }
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

bool Image::load(void* data, size_t size_bytes, CommandBuffer* buffer)
{
    // If device memory is local, need to load into a different resource and copy over
    if(_vk_memory_properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        Buffer* staging = _device->create_buffer(size_bytes, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        staging->load(data, size_bytes);

        buffer->copy_buffer_to_image(staging, this);

        _device->destroy_buffer(&staging);
    }
    else
    {
        void* mapped;
        vkMapMemory(_device->get_handle(), _vk_memory, 0, _size_bytes, 0, &mapped);
        memcpy(mapped, data, size_bytes);
        vkUnmapMemory(_device->get_handle(), _vk_memory);
    }

    return true;
}
