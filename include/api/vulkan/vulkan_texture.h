#ifndef REND_API_VULKAN_VULKAN_TEXTURE_H
#define REND_API_VULKAN_VULKAN_TEXTURE_H

#include "api/vulkan/vulkan_image_info.h"
#include "core/gpu_texture.h"

#include <string>

namespace rend
{

struct VulkanImageInfo;

class VulkanTexture : public GPUTexture
{
public:
    VulkanTexture(const std::string& name, RendHandle rend_handle, const TextureInfo& info, const VulkanImageInfo& vk_image_info);
    ~VulkanTexture(void) = default;

    const VulkanImageInfo& vk_image_info(void) const;

private:
    VulkanImageInfo _vk_image_info{};
};

}

#endif
