#include "api/vulkan/vulkan_framebuffer.h"

using namespace rend;

VulkanFramebuffer::VulkanFramebuffer(const std::string& name, const FramebufferInfo& info, RendHandle rend_handle, VkFramebuffer vk_handle)
    :
        Framebuffer(name, info, rend_handle),
        _vk_handle(vk_handle)
{
}

VkFramebuffer  VulkanFramebuffer::vk_handle(void) const
{
    return _vk_handle;
}
