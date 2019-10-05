#include "vulkan_depth_buffer.h"

using namespace rend;

VulkanDepthBuffer::VulkanDepthBuffer(DeviceContext& context)
    : VulkanGPUTexture(context)
{
}

VulkanDepthBuffer::~VulkanDepthBuffer(void)
{
}

StatusCode VulkanDepthBuffer::create_depth_buffer_api(uint32_t width, uint32_t height)
{
    VkExtent3D vk_extent               = VkExtent3D{ width, height, 1 };
    VkImageType vk_type                = VK_IMAGE_TYPE_2D;
    VkFormat vk_format                 = VK_FORMAT_D24_UNORM_S8_UINT;
    VkImageUsageFlags vk_usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkImageViewType vk_view_type       = VK_IMAGE_VIEW_TYPE_2D;
    VkImageAspectFlags vk_image_aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

    return create_texture_api(
        vk_extent, vk_type, vk_format, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_usage,vk_view_type, vk_image_aspect
    );
}
