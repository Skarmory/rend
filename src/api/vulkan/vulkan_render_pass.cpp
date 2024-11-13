#include "api/vulkan/vulkan_render_pass.h"

#include "api/vulkan/vulkan_command_buffer.h"
#include "core/draw_item.h"
#include "core/renderer.h"

using namespace rend;

VulkanRenderPass::VulkanRenderPass(const std::string& name, const RenderPassInfo& info, VkRenderPass vk_handle)
    :
        RenderPass(name, info),
        _vk_handle(vk_handle)
{
}

VulkanRenderPass::~VulkanRenderPass(void)
{
}

VkRenderPass VulkanRenderPass::vk_handle(void) const
{
    return _vk_handle;
}

void VulkanRenderPass::begin(CommandBuffer& command_buffer, const PerPassData& per_pass_data)
{
    auto& vk_cmd = static_cast<VulkanCommandBuffer&>(command_buffer);
    vk_cmd.begin_render_pass(*this, per_pass_data);
}

void VulkanRenderPass::end(CommandBuffer& command_buffer)
{
    auto& vk_cmd = static_cast<VulkanCommandBuffer&>(command_buffer);
    vk_cmd.end_render_pass();
}

void VulkanRenderPass::next_subpass(CommandBuffer& command_buffer)
{
    auto& vk_cmd = static_cast<VulkanCommandBuffer&>(command_buffer);
    vk_cmd.next_subpass();
}
