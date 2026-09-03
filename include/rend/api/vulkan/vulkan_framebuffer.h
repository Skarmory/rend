#ifndef REND_API_VULKAN_VULKAN_FRAMEBUFFER_H
#define REND_API_VULKAN_VULKAN_FRAMEBUFFER_H

#include "rend/core/framebuffer.h"

#include <string>
#include <vulkan/vulkan.h>

namespace rend
{

class VulkanFramebuffer : public Framebuffer
{

public:
    VulkanFramebuffer(const std::string& name, const FramebufferInfo& info, VkFramebuffer vk_handle);
    ~VulkanFramebuffer(void) = default;

    VkFramebuffer vk_handle(void) const;

private:
    VkFramebuffer _vk_handle{ VK_NULL_HANDLE };
};

}

#endif
