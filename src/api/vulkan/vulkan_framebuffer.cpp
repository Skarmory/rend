#include "api/vulkan/vulkan_framebuffer.h"

using namespace rend;

VulkanFramebuffer::VulkanFramebuffer(const std::string& name, const FramebufferInfo& info, VkFramebuffer vk_handle)
    :
        Framebuffer(name, info),
        _vk_handle(vk_handle)
{
}

VkFramebuffer  VulkanFramebuffer::vk_handle(void) const
{
    return _vk_handle;
}
