#ifndef REND_RENDERPASS_H
#define REND_RENDERPASS_H

#include <vulkan.h>
#include <vector>

#include "rend_defs.h"

namespace rend
{

class RenderPass
{
public:
    RenderPass(void) = default;
    ~RenderPass(void) = default;

    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    StatusCode create_render_pass(void);
    void destroy(void);

    uint32_t add_attachment_description(Format format, MSAASamples samples, LoadOp load_op, StoreOp store_op, LoadOp s_load_op, StoreOp s_store_op, ImageLayout initial, ImageLayout final);

    void     add_subpass(Synchronisation src, Synchronisation dst);
    void     add_subpass_colour_attachment_ref(uint32_t attach_slot, ImageLayout layout);
    void     add_subpass_colour_attachment_ref(uint32_t attach_slot);
    void     add_subpass_input_attachment_ref(uint32_t attach_slot, ImageLayout layout);
    void     add_subpass_resolve_attachment_ref(uint32_t attach_slot, ImageLayout layout);
    void     add_subpass_preserve_attachment_ref(uint32_t attach_slot);
    void     add_subpass_depth_stencil_attachment_ref(uint32_t attach_slot, ImageLayout layout);
    void     add_subpass_depth_stencil_attachment_ref(uint32_t attach_slot);

    VkRenderPass get_handle(void) const;
    const std::vector<VkAttachmentDescription>& get_attachment_descs(void) const;

private:
    struct Subpass
    {
        std::vector<VkAttachmentReference> vk_colour_attach_refs;
        std::vector<VkAttachmentReference> vk_input_attach_refs;
        std::vector<VkAttachmentReference> vk_resolve_attach_refs;
        std::vector<uint32_t>              vk_preserve_attach_refs;
        VkAttachmentReference              vk_depth_stencil_attach_ref;
        VkSubpassDescription               vk_subpass_desc;
        VkSubpassDependency                vk_subpass_dep;
        bool                               has_depth_stencil_attach { false };
    };

    std::vector<Subpass>                 _subpasses;
    std::vector<VkAttachmentDescription> _vk_attach_descs;
    VkRenderPass                         _vk_render_pass { VK_NULL_HANDLE };
};

}

#endif
