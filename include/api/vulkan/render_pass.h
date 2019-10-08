#ifndef REND_RENDERPASS_H
#define REND_RENDERPASS_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class DeviceContext;

class RenderPass
{
public:
    explicit RenderPass(DeviceContext& device);
    ~RenderPass(void);

    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    bool create_render_pass(const std::vector<VkAttachmentDescription>& attachment_descs, const std::vector<VkSubpassDescription>& subpass_descs, const std::vector<VkSubpassDependency>& subpass_deps);

    VkRenderPass get_handle(void) const;

private:
    DeviceContext& _context;
    VkRenderPass _vk_render_pass;
};

}

#endif
