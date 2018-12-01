#include "framebuffer.h"

#include "logical_device.h"
#include "render_pass.h"
#include "utils.h"

using namespace rend;

Framebuffer::Framebuffer(LogicalDevice* device, const RenderPass& render_pass, const std::vector<VkImageView>& attachments, VkExtent3D dimensions) : _device(device), _render_pass(&render_pass)
{
    _vk_create_info =
    {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .renderPass      = render_pass.get_handle(),
        .attachmentCount = 0,
        .pAttachments    = nullptr,
        .width           = 0,
        .height          = 0,
        .layers          = 0
    };

    _create(attachments, dimensions);
}

Framebuffer::~Framebuffer(void)
{
    _destroy();
}

VkFramebuffer Framebuffer::get_handle(void) const
{
    return _vk_framebuffer;
}

const RenderPass* Framebuffer::get_render_pass(void) const
{
    return _render_pass;
}

void Framebuffer::recreate(const std::vector<VkImageView>& attachments, VkExtent3D dimensions)
{
    _destroy();
    _create(attachments, dimensions);
}

void Framebuffer::_create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions)
{
    _vk_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    _vk_create_info.pAttachments = attachments.data();
    _vk_create_info.width = dimensions.width;
    _vk_create_info.height = dimensions.height;
    _vk_create_info.layers = dimensions.depth;

    VULKAN_DEATH_CHECK(vkCreateFramebuffer(_device->get_handle(), &_vk_create_info, nullptr, &_vk_framebuffer), "Failed to create framebuffer");
}

void Framebuffer::_destroy(void)
{
    vkDestroyFramebuffer(_device->get_handle(), _vk_framebuffer, nullptr);
}
