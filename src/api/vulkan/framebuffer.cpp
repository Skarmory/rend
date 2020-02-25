#include "framebuffer.h"

#include "depth_buffer.h"
#include "device_context.h"
#include "logical_device.h"
#include "render_pass.h"
#include "render_target.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

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

StatusCode Framebuffer::create_framebuffer(const RenderPass& render_pass, VkExtent3D dimensions)
{
    if(_vk_framebuffer != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    std::vector<VkImageView> attachments;
    for(RenderTarget* target : _render_targets)
        attachments.push_back(target->get_view());

    if(_depth_buffer)
        attachments.push_back(_depth_buffer->get_view());

    _render_pass = &render_pass;

    return _create(attachments, dimensions);
}

StatusCode Framebuffer::recreate(VkExtent3D dimensions)
{
    if(_vk_framebuffer == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

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

StatusCode Framebuffer::_create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions)
{
    VkFramebufferCreateInfo create_info = vulkan_helpers::gen_framebuffer_create_info();
    create_info.renderPass = _render_pass->get_handle();
    create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    create_info.pAttachments = attachments.data();
    create_info.width = dimensions.width;
    create_info.height = dimensions.height;
    create_info.layers = dimensions.depth;

    _vk_framebuffer = DeviceContext::instance().get_device()->create_framebuffer(create_info);
    if(_vk_framebuffer == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

void Framebuffer::_destroy(void)
{
    DeviceContext::instance().get_device()->destroy_framebuffer(_vk_framebuffer);
}

void Framebuffer::on_end_render_pass(void)
{
    const std::vector<VkAttachmentDescription>& descs = _render_pass->get_attachment_descs();
    for(uint32_t colour_attach_idx = 0; colour_attach_idx < _render_targets.size(); ++colour_attach_idx)
    {
        _render_targets[colour_attach_idx]->transition(descs[colour_attach_idx].finalLayout);
    }

    if(_depth_buffer)
    {
        _depth_buffer->transition(descs.back().finalLayout);
    }
}
