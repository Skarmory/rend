#include "vulkan_render_target.h"

#include "vulkan_helper_funcs.h"

using namespace rend;

StatusCode VulkanRenderTarget::create_render_target_api(uint32_t width, uint32_t height, Format format)
{
    VkExtent3D vk_extent               = VkExtent3D{ width, height, 1 };
    VkImageType vk_type                = vulkan_helpers::find_image_type(vk_extent);
    VkFormat vk_format                 = vulkan_helpers::convert_format(format);
    VkImageUsageFlags vk_usage         = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    VkImageViewType vk_view_type       = vulkan_helpers::find_image_view_type(vk_type, false, false);
    VkImageAspectFlags vk_image_aspect = vulkan_helpers::find_image_aspects(vk_format);

    return create_texture_api(
        vk_extent, vk_type, vk_format, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_usage,vk_view_type, vk_image_aspect
    );
}
