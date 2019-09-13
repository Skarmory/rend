#include "render_pass.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

RenderPass::RenderPass(LogicalDevice* device, const std::vector<VkAttachmentDescription>& attachment_descs, const std::vector<VkSubpassDescription>& subpass_descs, const std::vector<VkSubpassDependency>& subpass_deps)
    : _device(device)
{
    VkRenderPassCreateInfo create_info =
    {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .attachmentCount = static_cast<uint32_t>(attachment_descs.size()),
        .pAttachments    = attachment_descs.data(),
        .subpassCount    = static_cast<uint32_t>(subpass_descs.size()),
        .pSubpasses      = subpass_descs.data(),
        .dependencyCount = static_cast<uint32_t>(subpass_deps.size()),
        .pDependencies   = subpass_deps.data()
    };

    VULKAN_DEATH_CHECK(vkCreateRenderPass(_device->get_handle(), &create_info, nullptr, &_vk_render_pass), "Failed to create render pass");
}

RenderPass::~RenderPass(void)
{
    vkDestroyRenderPass(_device->get_handle(), _vk_render_pass, nullptr);
}

VkRenderPass RenderPass::get_handle(void) const
{
    return _vk_render_pass;
}
