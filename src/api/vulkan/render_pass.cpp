#include "render_pass.h"

#include "device_context.h"
#include "logical_device.h"

using namespace rend;

RenderPass::RenderPass(DeviceContext& context)
    : _context(context),
      _vk_render_pass(VK_NULL_HANDLE)
{
}

RenderPass::~RenderPass(void)
{
    vkDestroyRenderPass(_context.get_device()->get_handle(), _vk_render_pass, nullptr);
}

bool RenderPass::create_render_pass(const std::vector<VkAttachmentDescription>& attachment_descs, const std::vector<VkSubpassDescription>& subpass_descs, const std::vector<VkSubpassDependency>& subpass_deps)
{
    if(_vk_render_pass != VK_NULL_HANDLE)
        return false;

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

    if(vkCreateRenderPass(_context.get_device()->get_handle(), &create_info, nullptr, &_vk_render_pass) != VK_SUCCESS)
        return false;

    return true;
}

VkRenderPass RenderPass::get_handle(void) const
{
    return _vk_render_pass;
}
