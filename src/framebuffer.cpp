#include "framebuffer.h"

Framebuffer::Framebuffer(LogicalDevice* device, const RenderPass& render_pass, const std::vector<VkImageView>& attachments, VkExtent3D dimensions) : _device(device)
{
    VkFramebufferCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = render_pass.get_handle(),
        .attachmentCount = attachments.size(),
        .pAttachments = attachments.data(),
        .width = dimensions.width,
        .height = dimensions.height,
        .layers = dimensions.depth
    };

    VULKAN_DEATH_CHECK(vkCreateFramebuffer(_device->get_handle(), &create_info, nullptr, _vk_framebuffer), "Failed to create framebuffer");
}

Framebuffer::~Framebuffer(void)
{
    vkDestroyFramebuffer(_device->get_handle(), _vk_framebuffer, nullptr);
}

VkFramebuffer Framebuffer::get_handle(void) const
{
    return _vk_framebuffer;
}
