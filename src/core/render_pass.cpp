#include "render_pass.h"

#include "device_context.h"
#include "logical_device.h"

#include <cassert>

using namespace rend;

StatusCode RenderPass::create(const RenderPassInfo& info)
{
    assert(_handle == NULL_HANDLE && "Attempt to create a RenderPass that has already been created.");

    auto& ctx = DeviceContext::instance();
    _handle = ctx.create_render_pass(info);

    if(_handle == NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    _render_pass_info = info;

    return StatusCode::SUCCESS;
}

void RenderPass::destroy(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_render_pass(_handle);

    _handle = NULL_HANDLE;
    _render_pass_info = {};
}

//uint32_t RenderPass::add_attachment_description(Format format, MSAASamples samples, LoadOp load_op, StoreOp store_op, LoadOp s_load_op, StoreOp s_store_op, ImageLayout initial, ImageLayout final)
//{
//    uint32_t attach_slot = _vk_attach_descs.size();
//
//    VkAttachmentDescription desc;
//    desc.flags          = 0;
//    desc.format         = vulkan_helpers::convert_format(format);
//    desc.samples        = vulkan_helpers::convert_sample_count(samples);
//    desc.loadOp         = vulkan_helpers::convert_load_op(load_op);
//    desc.storeOp        = vulkan_helpers::convert_store_op(store_op);
//    desc.stencilLoadOp  = vulkan_helpers::convert_load_op(s_load_op);
//    desc.stencilStoreOp = vulkan_helpers::convert_store_op(s_store_op);
//    desc.initialLayout  = vulkan_helpers::convert_image_layout(initial);
//    desc.finalLayout    = vulkan_helpers::convert_image_layout(final);
//
//    _vk_attach_descs.push_back(desc);
//
//    return attach_slot;
//}
//
//void RenderPass::add_subpass(Synchronisation src, Synchronisation dst)
//{
//    Subpass subpass;
//    subpass.vk_depth_stencil_attach_ref = {};
//    subpass.vk_subpass_desc = {};
//
//    subpass.vk_subpass_dep = {};
//    subpass.vk_subpass_dep.srcStageMask  = vulkan_helpers::convert_pipeline_stages(src.stages);
//    subpass.vk_subpass_dep.dstStageMask  = vulkan_helpers::convert_pipeline_stages(dst.stages);
//    subpass.vk_subpass_dep.srcAccessMask = vulkan_helpers::convert_memory_accesses(src.accesses);
//    subpass.vk_subpass_dep.dstAccessMask = vulkan_helpers::convert_memory_accesses(dst.accesses);
//
//    _subpasses.push_back(subpass);
//}
//
//void RenderPass::add_subpass_colour_attachment_ref(uint32_t attach_slot, ImageLayout layout)
//{
//    _subpasses.back().vk_colour_attach_refs.push_back({
//        attach_slot, vulkan_helpers::convert_image_layout(layout)
//    });
//}
//
//void RenderPass::add_subpass_colour_attachment_ref(uint32_t attach_slot)
//{
//    _subpasses.back().vk_colour_attach_refs.push_back({
//        attach_slot, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
//    });
//}
//
//void RenderPass::add_subpass_input_attachment_ref(uint32_t attach_slot, ImageLayout layout)
//{
//    _subpasses.back().vk_input_attach_refs.push_back({
//        attach_slot, vulkan_helpers::convert_image_layout(layout)
//    });
//}
//
//void RenderPass::add_subpass_resolve_attachment_ref(uint32_t attach_slot, ImageLayout layout)
//{
//    _subpasses.back().vk_resolve_attach_refs.push_back({
//        attach_slot, vulkan_helpers::convert_image_layout(layout)
//    });
//}
//
//void RenderPass::add_subpass_preserve_attachment_ref(uint32_t attach_slot)
//{
//    _subpasses.back().vk_preserve_attach_refs.push_back(attach_slot);
//}
//
//void RenderPass::add_subpass_depth_stencil_attachment_ref(uint32_t attach_slot, ImageLayout layout)
//{
//    _subpasses.back().vk_depth_stencil_attach_ref = {
//        attach_slot, vulkan_helpers::convert_image_layout(layout)
//    };
//
//    _subpasses.back().has_depth_stencil_attach = true;
//}
//
//void RenderPass::add_subpass_depth_stencil_attachment_ref(uint32_t attach_slot)
//{
//    _subpasses.back().vk_depth_stencil_attach_ref = {
//        attach_slot, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
//    };
//
//    _subpasses.back().has_depth_stencil_attach = true;
//}

RenderPassHandle RenderPass::handle(void) const
{
    return _handle;
}

//const std::vector<VkAttachmentDescription>& RenderPass::get_attachment_descs(void) const
//{
//    return _vk_attach_descs;
//}
