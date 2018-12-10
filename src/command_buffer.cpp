#include "command_buffer.h"

#include "framebuffer.h"
#include "render_pass.h"
#include "utils.h"

#include <iostream>

using namespace rend;

CommandBuffer::CommandBuffer(VkCommandBuffer vk_command_buffer, uint32_t index)
    : _vk_command_buffer(vk_command_buffer), _index(index)
{
    std::cout << "Constructing command buffer" << std::endl;
}

CommandBuffer::~CommandBuffer(void)
{
    std::cout << "Destructing command buffer" << std::endl;
}

VkCommandBuffer CommandBuffer::get_handle(void) const
{
    return _vk_command_buffer;
}

void CommandBuffer::begin(void)
{
    VkCommandBufferBeginInfo info =
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .pInheritanceInfo = nullptr
    };

    VULKAN_DEATH_CHECK(vkBeginCommandBuffer(_vk_command_buffer, &info), "Failed to begin command buffer");
}

void CommandBuffer::end(void)
{
    vkEndCommandBuffer(_vk_command_buffer);
}

void CommandBuffer::reset(void)
{
    VULKAN_DEATH_CHECK(vkResetCommandBuffer(_vk_command_buffer, 0), "Failed to reset command buffer");
}

void CommandBuffer::begin_render_pass(const RenderPass& render_pass, Framebuffer* framebuffer, VkRect2D render_area, const std::vector<VkClearValue>& clear_values)
{
    VkRenderPassBeginInfo info =
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = render_pass.get_handle(),
        .framebuffer = framebuffer->get_handle(),
        .renderArea = render_area,
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data()
    };

    vkCmdBeginRenderPass(_vk_command_buffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::end_render_pass(void)
{
    vkCmdEndRenderPass(_vk_command_buffer);
}

void CommandBuffer::set_viewport(const VkViewport& viewport)
{
    vkCmdSetViewport(_vk_command_buffer, 0, 1, &viewport);
}

void CommandBuffer::set_scissors(const VkRect2D& scissor)
{
    vkCmdSetScissor(_vk_command_buffer, 0, 1, &scissor);
}
