#include "vulkan_gpu_texture.h"

#include "command_buffer.h"
#include "command_pool.h"
#include "device_context.h"
#include "fence.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

using namespace rend;

VulkanGPUTexture::~VulkanGPUTexture(void)
{
    destroy_texture_api();
}

StatusCode VulkanGPUTexture::create_texture_api(VkExtent3D extent, VkImageType type, VkFormat format, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkMemoryPropertyFlags memory_properties, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspects)
{
    if(_vk_image != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    if(!_create_vk_image(type, format, extent, mip_levels, array_layers, sample_count, tiling, usage))
    {
        return StatusCode::IMAGE_CREATE_FAILURE;
    }

    if(!_alloc_vk_memory(memory_properties))
    {
        return StatusCode::MEMORY_ALLOC_FAILURE;
    }

    if(static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->bind_image_memory(_vk_image, _vk_memory) != VK_SUCCESS)
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
    _vk_sample_count = sample_count;
    _vk_tiling = tiling;
    _vk_usage = usage;
    _vk_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    _loaded = false;

    return StatusCode::SUCCESS;
}

void VulkanGPUTexture::destroy_texture_api(void)
{
    auto& context = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    context.get_device()->destroy_sampler(_vk_sampler);
    context.get_device()->destroy_image_view(_vk_image_view);
    if(!_swapchain_image)
    {
        context.get_device()->free_memory(_vk_memory);
        context.get_device()->destroy_image(_vk_image);
    }

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

VkSampleCountFlagBits VulkanGPUTexture::get_sample_count(void) const
{
    return _vk_sample_count;
}

VkDeviceMemory VulkanGPUTexture::get_memory(void) const
{
    return _vk_memory;
}

VkMemoryPropertyFlags VulkanGPUTexture::get_memory_properties(void) const
{
    return _vk_memory_properties;
}

bool VulkanGPUTexture::_create_vk_image(VkImageType type, VkFormat format, VkExtent3D extent, uint32_t mip_levels, uint32_t array_layers, VkSampleCountFlagBits sample_count, VkImageTiling tiling, VkImageUsageFlags usage)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    uint32_t queue_family_index = ctx.get_device()->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkImageCreateInfo create_info     = vulkan_helpers::gen_image_create_info();
    create_info.format                = format;
    create_info.imageType             = type;
    create_info.extent                = extent;
    create_info.mipLevels             = mip_levels;
    create_info.arrayLayers           = array_layers;
    create_info.samples               = sample_count;
    create_info.tiling                = tiling;
    create_info.usage                 = usage;
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices   = &queue_family_index;
    create_info.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

    _vk_image = ctx.get_device()->create_image(create_info);
    if(_vk_image == VK_NULL_HANDLE)
        return false;

    return true;
}

bool VulkanGPUTexture::_alloc_vk_memory(VkMemoryPropertyFlags memory_properties)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    VkMemoryRequirements memory_reqs = ctx.get_device()->get_image_memory_reqs(_vk_image);
    
    VkMemoryAllocateInfo alloc_info = vulkan_helpers::gen_memory_allocate_info();
    alloc_info.allocationSize       = memory_reqs.size;
    alloc_info.memoryTypeIndex      = ctx.get_device()->find_memory_type(memory_reqs.memoryTypeBits, memory_properties);

    _vk_memory = ctx.get_device()->allocate_memory(alloc_info);
    if(_vk_memory == VK_NULL_HANDLE)
        return false;

    _bytes = static_cast<size_t>(memory_reqs.size);

    return true;
}

bool VulkanGPUTexture::_create_vk_image_view(VkFormat format, VkImageViewType view_type, VkImageAspectFlags aspects, uint32_t mip_levels, uint32_t array_layers)
{
    VkImageViewCreateInfo image_view_info = vulkan_helpers::gen_image_view_create_info();
    image_view_info.image                           = _vk_image;
    image_view_info.viewType                        = view_type;
    image_view_info.format                          = format;
    image_view_info.subresourceRange.aspectMask     = aspects;
    image_view_info.subresourceRange.baseMipLevel   = 0;
    image_view_info.subresourceRange.levelCount     = mip_levels;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount     = array_layers;

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_image_view = ctx.get_device()->create_image_view(image_view_info);
    if(_vk_image_view == VK_NULL_HANDLE)
        return false;

    return true;
}

bool VulkanGPUTexture::_create_vk_sampler(void)
{
    VkSamplerCreateInfo sampler_info     = vulkan_helpers::gen_sampler_create_info();
    sampler_info.magFilter               = VK_FILTER_LINEAR;
    sampler_info.minFilter               = VK_FILTER_LINEAR;
    sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.mipLodBias              = 1.0f;
    sampler_info.anisotropyEnable        = VK_FALSE;
    sampler_info.maxAnisotropy           = 1.0f;
    sampler_info.compareEnable           = VK_FALSE;
    sampler_info.compareOp               = VK_COMPARE_OP_ALWAYS;
    sampler_info.minLod                  = 1.0f;
    sampler_info.maxLod                  = 1.0f;
    sampler_info.borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_sampler = ctx.get_device()->create_sampler(sampler_info);
    if(_vk_sampler == VK_NULL_HANDLE)
        return false;

    return true;
}

void VulkanGPUTexture::transition(VkImageLayout final_layout)
{
    // TODO: Put transition logic in here
    _vk_layout = final_layout;
}
