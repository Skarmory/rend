#include "render_pass.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

using namespace rend;

RenderPass::~RenderPass(void)
{
    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->destroy_render_pass(_vk_render_pass);
}

StatusCode RenderPass::create_render_pass(void)
{
    if(_vk_render_pass != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    std::vector<VkSubpassDescription> subpass_descs;
    std::vector<VkSubpassDependency>  subpass_deps;

    for(uint32_t subpass_idx = 0; subpass_idx < _subpasses.size(); ++subpass_idx)
    {
        Subpass& subpass = _subpasses[subpass_idx];
        subpass.vk_subpass_desc.flags = 0;
        subpass.vk_subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        subpass.vk_subpass_desc.colorAttachmentCount = subpass.vk_colour_attach_refs.size();
        if(subpass.vk_subpass_desc.colorAttachmentCount > 0)
            subpass.vk_subpass_desc.pColorAttachments = subpass.vk_colour_attach_refs.data();

        subpass.vk_subpass_desc.inputAttachmentCount = subpass.vk_input_attach_refs.size();
        if(subpass.vk_subpass_desc.inputAttachmentCount > 0)
            subpass.vk_subpass_desc.pInputAttachments = subpass.vk_input_attach_refs.data();

        subpass.vk_subpass_desc.preserveAttachmentCount = subpass.vk_preserve_attach_refs.size();
        if(subpass.vk_subpass_desc.preserveAttachmentCount > 0)
            subpass.vk_subpass_desc.pPreserveAttachments = subpass.vk_preserve_attach_refs.data();

        if(subpass.vk_resolve_attach_refs.size() > 0)
            subpass.vk_subpass_desc.pResolveAttachments = subpass.vk_resolve_attach_refs.data();

        if(subpass.has_depth_stencil_attach)
            subpass.vk_subpass_desc.pDepthStencilAttachment = &subpass.vk_depth_stencil_attach_ref;

        subpass_descs.push_back(subpass.vk_subpass_desc);

        subpass.vk_subpass_dep.srcSubpass = subpass_idx - 1;
        subpass.vk_subpass_dep.dstSubpass = subpass_idx;
        subpass_deps.push_back(subpass.vk_subpass_dep);
    }

    subpass_deps.front().srcSubpass = VK_SUBPASS_EXTERNAL;

    subpass_deps.push_back({
        subpass_deps.back().dstSubpass,
        VK_SUBPASS_EXTERNAL,
        subpass_deps.back().dstStageMask,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        subpass_deps.back().dstAccessMask,
        VK_ACCESS_MEMORY_READ_BIT,
        static_cast<VkDependencyFlags>(0)
    });

    VkRenderPassCreateInfo create_info = vulkan_helpers::gen_render_pass_create_info();
    create_info.attachmentCount = static_cast<uint32_t>(_vk_attach_descs.size());
    create_info.pAttachments    = _vk_attach_descs.data();
    create_info.subpassCount    = static_cast<uint32_t>(subpass_descs.size());
    create_info.pSubpasses      = subpass_descs.data();
    create_info.dependencyCount = static_cast<uint32_t>(subpass_deps.size());
    create_info.pDependencies   = subpass_deps.data();

    _vk_render_pass = static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->create_render_pass(create_info);

    if(_vk_render_pass == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

uint32_t RenderPass::add_attachment_description(Format format, MSAASamples samples, LoadOp load_op, StoreOp store_op, LoadOp s_load_op, StoreOp s_store_op, ImageLayout initial, ImageLayout final)
{
    uint32_t attach_slot = _vk_attach_descs.size();

    VkAttachmentDescription desc;
    desc.flags          = 0;
    desc.format         = vulkan_helpers::convert_format(format);
    desc.samples        = vulkan_helpers::convert_sample_count(samples);
    desc.loadOp         = vulkan_helpers::convert_load_op(load_op);
    desc.storeOp        = vulkan_helpers::convert_store_op(store_op);
    desc.stencilLoadOp  = vulkan_helpers::convert_load_op(s_load_op);
    desc.stencilStoreOp = vulkan_helpers::convert_store_op(s_store_op);
    desc.initialLayout  = vulkan_helpers::convert_image_layout(initial);
    desc.finalLayout    = vulkan_helpers::convert_image_layout(final);

    _vk_attach_descs.push_back(desc);

    return attach_slot;
}

void RenderPass::add_subpass(Synchronisation src, Synchronisation dst)
{
    Subpass subpass;
    subpass.vk_depth_stencil_attach_ref = {};
    subpass.vk_subpass_desc = {};

    subpass.vk_subpass_dep = {};
    subpass.vk_subpass_dep.srcStageMask  = vulkan_helpers::convert_pipeline_stages(src.stages);
    subpass.vk_subpass_dep.dstStageMask  = vulkan_helpers::convert_pipeline_stages(dst.stages);
    subpass.vk_subpass_dep.srcAccessMask = vulkan_helpers::convert_memory_accesses(src.accesses);
    subpass.vk_subpass_dep.dstAccessMask = vulkan_helpers::convert_memory_accesses(dst.accesses);

    _subpasses.push_back(subpass);
}

void RenderPass::add_subpass_colour_attachment_ref(uint32_t attach_slot, ImageLayout layout)
{
    _subpasses.back().vk_colour_attach_refs.push_back({
        attach_slot, vulkan_helpers::convert_image_layout(layout)
    });
}

void RenderPass::add_subpass_colour_attachment_ref(uint32_t attach_slot)
{
    _subpasses.back().vk_colour_attach_refs.push_back({
        attach_slot, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    });
}

void RenderPass::add_subpass_input_attachment_ref(uint32_t attach_slot, ImageLayout layout)
{
    _subpasses.back().vk_input_attach_refs.push_back({
        attach_slot, vulkan_helpers::convert_image_layout(layout)
    });
}

void RenderPass::add_subpass_resolve_attachment_ref(uint32_t attach_slot, ImageLayout layout)
{
    _subpasses.back().vk_resolve_attach_refs.push_back({
        attach_slot, vulkan_helpers::convert_image_layout(layout)
    });
}

void RenderPass::add_subpass_preserve_attachment_ref(uint32_t attach_slot)
{
    _subpasses.back().vk_preserve_attach_refs.push_back(attach_slot);
}

void RenderPass::add_subpass_depth_stencil_attachment_ref(uint32_t attach_slot, ImageLayout layout)
{
    _subpasses.back().vk_depth_stencil_attach_ref = {
        attach_slot, vulkan_helpers::convert_image_layout(layout)
    };

    _subpasses.back().has_depth_stencil_attach = true;
}

void RenderPass::add_subpass_depth_stencil_attachment_ref(uint32_t attach_slot)
{
    _subpasses.back().vk_depth_stencil_attach_ref = {
        attach_slot, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    _subpasses.back().has_depth_stencil_attach = true;
}

VkRenderPass RenderPass::get_handle(void) const
{
    return _vk_render_pass;
}

const std::vector<VkAttachmentDescription>& RenderPass::get_attachment_descs(void) const
{
    return _vk_attach_descs;
}
