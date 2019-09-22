#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;

class RenderPass
{
    friend class LogicalDevice;

public:
    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    VkRenderPass get_handle(void) const;

private:
    RenderPass(LogicalDevice* device, const std::vector<VkAttachmentDescription>& attachment_descs, const std::vector<VkSubpassDescription>& subpass_descs, const std::vector<VkSubpassDependency>& subpass_deps);
    ~RenderPass(void);

private:
    VkRenderPass _vk_render_pass;

    LogicalDevice* _device;

};

}

#endif
