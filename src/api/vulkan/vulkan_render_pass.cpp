#include "api/vulkan/vulkan_render_pass.h"

#include "api/vulkan/vulkan_command_buffer.h"
#include "core/draw_item.h"
#include "core/renderer.h"

using namespace rend;

VulkanRenderPass::VulkanRenderPass(const std::string& name, const RenderPassInfo& info, VkRenderPass vk_handle, RendHandle rend_handle)
    :
        RenderPass(name, info, rend_handle),
        _vk_handle(vk_handle)
{
    //for(int i = 0; i < subpasses.size(); ++i)
    //{
    //    std::string subpass_name = name + ", Subpass: " + std::to_string(i);
    //    _subpasses.emplace_back(
    //        rr->create_sub_pass(subpass_name, _info.subpasses[i].shader_set_handle, _handle, i)
    //    );
    //}
}

VulkanRenderPass::~VulkanRenderPass(void)
{
    //auto& ctx = *RendService::device_context();
    //ctx.destroy_render_pass(_handle);
}

VkRenderPass VulkanRenderPass::vk_handle(void) const
{
    return _vk_handle;
}

void VulkanRenderPass::add_subpass(const std::string& name, const SubPassInfo& info)
{
    auto* subpass = Renderer::get_instance().create_sub_pass(name, info);
    _subpasses.push_back(subpass);
}

const std::vector<SubPass*>& VulkanRenderPass::get_subpasses(void) const
{
    return _subpasses;
}

bool VulkanRenderPass::has_next_subpass(void) const
{
    return _current_subpass + 1 != _subpasses.size();
}

void VulkanRenderPass::begin(CommandBuffer& command_buffer, PerPassData& per_pass)
{
    auto& vk_cmd = static_cast<VulkanCommandBuffer&>(command_buffer);
    vk_cmd.begin_render_pass(*this, *per_pass.framebuffer, per_pass.render_area, per_pass.colour_clear, per_pass.depth_clear);
    _subpasses[_current_subpass]->begin(command_buffer);
}

void VulkanRenderPass::next_subpass(CommandBuffer& command_buffer)
{
    if(has_next_subpass())
    {
        auto& vk_cmd = static_cast<VulkanCommandBuffer&>(command_buffer);
        _current_subpass++;
        vk_cmd.next_subpass();
        _subpasses[_current_subpass]->begin(command_buffer);
    }
}

void VulkanRenderPass::end(CommandBuffer& command_buffer)
{
    auto& vk_cmd = static_cast<VulkanCommandBuffer&>(command_buffer);
    vk_cmd.end_render_pass();
    _current_subpass = 0;
}
