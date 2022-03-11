#include "core/framebuffer.h"

#include "core/device_context.h"
#include "core/gpu_texture.h"
#include "core/render_pass.h"

#include <cassert>

using namespace rend;

StatusCode Framebuffer::create(const FramebufferInfo& info)
{
    assert(_handle == NULL_HANDLE && "Attempt to create a Framebuffer that has already been created.");

    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_framebuffer(info);
    if(_handle == NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    _framebuffer_info = info;

    return StatusCode::SUCCESS;
}

void Framebuffer::destroy(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_framebuffer(_handle);

    _handle = NULL_HANDLE;
    _framebuffer_info = {};
}

FramebufferHandle Framebuffer::handle(void) const
{
    return _handle;
}

//const RenderPass* Framebuffer::render_pass(void) const
//{
//    return _render_pass;
//}
//
//Texture2DHandle Framebuffer::depth_buffer(void) const
//{
//    return _depth_buffer;
//}

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
