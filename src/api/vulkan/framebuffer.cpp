#include "framebuffer.h"

#include "device_context.h"
#include "gpu_texture.h"
#include "logical_device.h"
#include "render_pass.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

using namespace rend;

bool Framebuffer::add_render_target(Texture2DHandle target)
{
    if (_vk_framebuffer != VK_NULL_HANDLE)
    {
        return false;
    }

    _render_targets.push_back(target);

    return true;
}

bool Framebuffer::set_depth_buffer(GPUTexture& buffer)
{
    if(_vk_framebuffer != VK_NULL_HANDLE)
        return false;

    _depth_buffer = &buffer;

    return true;
}

StatusCode Framebuffer::create_framebuffer(const RenderPass& render_pass, VkExtent3D dimensions)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    if (_vk_framebuffer != VK_NULL_HANDLE)
    {
        return StatusCode::ALREADY_CREATED;
    }

    std::vector<VkImageView> attachments;

    for (Texture2DHandle target : _render_targets)
    {
        attachments.push_back(ctx.get_image_view(target));
    }

    if (_depth_buffer)
    {
        attachments.push_back(ctx.get_image_view(_depth_buffer->get_handle()));
    }

    _render_pass = &render_pass;

    return _create(attachments, dimensions);
}

StatusCode Framebuffer::recreate(VkExtent3D dimensions)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    if (_vk_framebuffer == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    std::vector<VkImageView> attachments;

    for (Texture2DHandle target : _render_targets)
    {
        attachments.push_back(ctx.get_image_view(target));
    }

    if (_depth_buffer)
    {
        attachments.push_back(ctx.get_image_view(_depth_buffer->get_handle()));
    }

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

const GPUTexture* Framebuffer::get_depth_buffer(void) const
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

    _vk_framebuffer = static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->create_framebuffer(create_info);
    if(_vk_framebuffer == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

void Framebuffer::destroy(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_framebuffer(_vk_framebuffer);
    _vk_framebuffer = VK_NULL_HANDLE;
    _render_targets.clear();
    _depth_buffer = NULL_HANDLE;
    _render_pass = nullptr;
}

void Framebuffer::on_end_render_pass(void)
{
    //const std::vector<VkAttachmentDescription>& descs = _render_pass->get_attachment_descs();
    //for(uint32_t colour_attach_idx = 0; colour_attach_idx < _render_targets.size(); ++colour_attach_idx)
    //{
    //    _render_targets[colour_attach_idx]->transition(
    //        vulkan_helpers::convert_image_layout(descs[colour_attach_idx].finalLayout)
    //    );
    //}

    //if(_depth_buffer)
    //{
    //    _depth_buffer->transition(
    //        vulkan_helpers::convert_image_layout(descs.back().finalLayout)
    //    );
    //}
}
