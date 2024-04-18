#include "api/vulkan/vulkan_texture.h"

using namespace rend;

VulkanTexture::VulkanTexture(const std::string& name, RendHandle rend_handle, const TextureInfo& info, const VulkanImageInfo& vk_image_info)
    :
        GPUTexture(name, info, rend_handle),
        _vk_image_info(vk_image_info)
{
}

const VulkanImageInfo& VulkanTexture::vk_image_info(void) const
{
    return _vk_image_info;
}

