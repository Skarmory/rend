#include "vulkan_gpu_texture.h"

#include "command_buffer.h"
#include "command_pool.h"
#include "device_context.h"
#include "fence.h"
#include "logical_device.h"

using namespace rend;

VulkanGPUTexture::VulkanGPUTexture(DeviceContext& context)
    : _context(context),
      _loaded(false),
      _vk_image(VK_NULL_HANDLE),
      _vk_image_view(VK_NULL_HANDLE),
      _vk_sampler(VK_NULL_HANDLE),
      _vk_memory_properties(0),
      _vk_type(VK_IMAGE_TYPE_MAX_ENUM),
      _vk_format(VK_FORMAT_MAX_ENUM),
      _mip_levels(0),
      _array_layers(0),
      _vk_samples(VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM),
      _vk_tiling(VK_IMAGE_TILING_MAX_ENUM),
      _vk_usage(0),
      _vk_layout(VK_IMAGE_LAYOUT_MAX_ENUM)
{
}

VulkanGPUTexture::~VulkanGPUTexture(void)
{
    destroy_texture_api();
}

StatusCode VulkanGPUTexture::create_texture_api(VkExtent3D extent, VkImageType type, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspects)
{
    if(_vk_image != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    if(!_create_vk_image(type, format, extent, mip_levels, array_layers, samples, tiling, usage))
    {
        return StatusCode::IMAGE_CREATE_FAILURE;
    }

    if(!_alloc_vk_memory(memory_properties))
    {
        return StatusCode::MEMORY_ALLOC_FAILURE;
    }

    if(vkBindImageMemory(_context.get_device()->get_handle(), _vk_image, _vk_memory, 0) != VK_SUCCESS)
    {
        return StatusCode::MEMORY_BIND_IMAGE_FAILURE;
    }

    if(!_create_vk_image_view(format, view_type, aspects, mip_levels, array_layers))
    {
        return StatusCode::IMAGE_VIEW_CREATE_FAILURE;
    }

    if(!_create_vk_sampler())
    {
        return StatusCode::SAMPLER_CREATE_FAILURE;
    }

    _vk_memory_properties = memory_properties;
    _vk_type = type;
    _vk_format = format;
    _mip_levels = mip_levels;
    _array_layers = array_layers;
    _vk_samples = samples;
    _vk_tiling = tiling;
    _vk_usage = usage;
    _vk_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    _loaded = false;

    return StatusCode::SUCCESS;
}

void VulkanGPUTexture::destroy_texture_api(void)
{
    vkDestroySampler(_context.get_device()->get_handle(), _vk_sampler, nullptr);
    vkDestroyImageView(_context.get_device()->get_handle(), _vk_image_view, nullptr);
    vkFreeMemory(_context.get_device()->get_handle(), _vk_memory, nullptr);
    vkDestroyImage(_context.get_device()->get_handle(), _vk_image, nullptr);

    _vk_sampler = VK_NULL_HANDLE;
    _vk_image_view = VK_NULL_HANDLE;
    _vk_memory = VK_NULL_HANDLE;
    _vk_image = VK_NULL_HANDLE;
}

VkImage VulkanGPUTexture::get_handle(void) const
{
    return _vk_image;
}

VkImageView VulkanGPUTexture::get_view(void) const
{
    return _vk_image_view;
}

VkSampler VulkanGPUTexture::get_sampler(void) const
{
    return _vk_sampler;
}

VkImageLayout VulkanGPUTexture::get_layout(void) const
{
    return _vk_layout;
}

VkExtent3D VulkanGPUTexture::get_extent(void) const
{
    return VkExtent3D{ width(), height(), depth() };
}

VkFormat VulkanGPUTexture::get_vk_format(void) const
{
    return _vk_format;
}

uint32_t VulkanGPUTexture::get_array_layers(void) const
{
    return _array_layers;
}

uint32_t VulkanGPUTexture::get_mip_levels(void) const
{
    return _mip_levels;
}

VkDeviceMemory VulkanGPUTexture::get_memory(void) const
{
    return _vk_memory;
}

VkMemoryPropertyFlags VulkanGPUTexture::get_memory_properties(void) const
{
    return _vk_memory_properties;
}

bool VulkanGPUTexture::_create_vk_image(VkImageType type, VkFormat format, VkExtent3D extent, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage)
{
    uint32_t queue_family_index = _context.get_device()->get_queue_family(QueueType::GRAPHICS)->get_index();

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

    if(vkCreateImage(_context.get_device()->get_handle(), &image_info, nullptr, &_vk_image) != VK_SUCCESS)
        return false;

    return true;
}

bool VulkanGPUTexture::_alloc_vk_memory(VkMemoryPropertyFlags memory_properties)
{
    VkMemoryRequirements memory_reqs;
    vkGetImageMemoryRequirements(_context.get_device()->get_handle(), _vk_image, &memory_reqs);
    
    VkMemoryAllocateInfo alloc_info =
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memory_reqs.size,
        .memoryTypeIndex = _context.get_device()->find_memory_type(memory_reqs.memoryTypeBits, memory_properties)
    };

    if(vkAllocateMemory(_context.get_device()->get_handle(), &alloc_info, nullptr, &_vk_memory) != VK_SUCCESS)
        return false;

    _bytes = memory_reqs.size;

    return true;
}

bool VulkanGPUTexture::_create_vk_image_view(VkFormat format, VkImageViewType view_type, VkImageAspectFlags aspects, uint32_t mip_levels, uint32_t array_layers)
{
    VkImageViewCreateInfo image_view_info =
    {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .image            = _vk_image,
        .viewType         = view_type,
        .format           = format,
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
            .levelCount     = mip_levels,
            .baseArrayLayer = 0,
            .layerCount     = array_layers
        }
    };

    if(vkCreateImageView(_context.get_device()->get_handle(), &image_view_info, nullptr, &_vk_image_view) != VK_SUCCESS)
        return false;

    return true;
}

bool VulkanGPUTexture::_create_vk_sampler(void)
{
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

    if(vkCreateSampler(_context.get_device()->get_handle(), &sampler_info, nullptr, &_vk_sampler) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void VulkanGPUTexture::transition(VkImageLayout final_layout)
{
    // TODO: Put transition logic in here
    _vk_layout = final_layout;
}
