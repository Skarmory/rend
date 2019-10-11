#include "framebuffer.h"

#include "depth_buffer.h"
#include "device_context.h"
#include "logical_device.h"
#include "render_pass.h"
#include "render_target.h"

using namespace rend;

Framebuffer::Framebuffer(DeviceContext& context)
    : _context(context),
      _render_pass(nullptr),
      _vk_create_info({}),
      _vk_framebuffer(VK_NULL_HANDLE)
{
}

Framebuffer::~Framebuffer(void)
{
    _destroy();
}

bool Framebuffer::add_render_target(RenderTarget& target)
{
    if(_vk_framebuffer != VK_NULL_HANDLE)
        return false;

    _render_targets.push_back(&target);

    return true;
}

bool Framebuffer::set_depth_buffer(DepthBuffer& buffer)
{
    if(_vk_framebuffer != VK_NULL_HANDLE)
        return false;

    _depth_buffer = &buffer;

    return true;
}

bool Framebuffer::create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& attachments, VkExtent3D dimensions)
{
    if(_vk_framebuffer != VK_NULL_HANDLE)
        return false;

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

    _render_pass = &render_pass;

    return _create(attachments, dimensions);
}

bool Framebuffer::recreate(const std::vector<VkImageView>& attachments, VkExtent3D dimensions)
{
    if(_vk_framebuffer == VK_NULL_HANDLE)
        return false;

    _destroy();
    return _create(attachments, dimensions);
}

bool Framebuffer::create_framebuffer(const RenderPass& render_pass, VkExtent3D dimensions)
{
    if(_vk_framebuffer != VK_NULL_HANDLE)
        return false;

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

    std::vector<VkImageView> attachments;
    for(RenderTarget* target : _render_targets)
        attachments.push_back(target->get_view());

    if(_depth_buffer)
        attachments.push_back(_depth_buffer->get_view());

    _render_pass = &render_pass;

    return _create(attachments, dimensions);
}

bool Framebuffer::recreate(VkExtent3D dimensions)
{
    if(_vk_framebuffer == VK_NULL_HANDLE)
        return false;

    std::vector<VkImageView> attachments;
    for(RenderTarget* target : _render_targets)
        attachments.push_back(target->get_view());

    if(_depth_buffer)
        attachments.push_back(_depth_buffer->get_view());

    _destroy();
    return _create(attachments, dimensions);
}

VkFramebuffer Framebuffer::get_handle(void) const
{
    return _vk_framebuffer;
}

const VkFramebufferCreateInfo& Framebuffer::get_vk_create_info(void) const
{
    return _vk_create_info;
}

const RenderPass* Framebuffer::get_render_pass(void) const
{
    return _render_pass;
}

const std::vector<RenderTarget*>& Framebuffer::get_render_targets(void) const
{
    return _render_targets;
}

const DepthBuffer* Framebuffer::get_depth_buffer(void) const
{
    return _depth_buffer;
}

bool Framebuffer::_create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions)
{
    _vk_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    _vk_create_info.pAttachments = attachments.data();
    _vk_create_info.width = dimensions.width;
    _vk_create_info.height = dimensions.height;
    _vk_create_info.layers = dimensions.depth;

    if(vkCreateFramebuffer(_context.get_device()->get_handle(), &_vk_create_info, nullptr, &_vk_framebuffer) != VK_SUCCESS)
        return false;

    return true;
}

void Framebuffer::_destroy(void)
{
    vkDestroyFramebuffer(_context.get_device()->get_handle(), _vk_framebuffer, nullptr);
}
