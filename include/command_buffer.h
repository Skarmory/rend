#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandPool;
class DescriptorSet;
class Framebuffer;
class PipelineLayout;
class RenderPass;

class CommandBuffer
{
    friend class CommandPool;

public:

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&)      = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&)      = delete;

    VkCommandBuffer get_handle(void) const;

    void begin(void);
    void end(void);
    void reset(void);

    void begin_render_pass(const RenderPass& render_pass, Framebuffer* framebuffer, VkRect2D render_area, const std::vector<VkClearValue>& clear_values);
    void end_render_pass(void);

    void set_viewport(const VkViewport& viewport);
    void set_scissors(const VkRect2D& scissor);

    void bind_descriptor_sets(VkPipelineBindPoint bind_point, const PipelineLayout& layout, const std::vector<DescriptorSet*>& sets);

private:

    CommandBuffer(VkCommandBuffer vk_command_buffer, uint32_t index);
    ~CommandBuffer(void);

private:
    VkCommandBuffer _vk_command_buffer;
    uint32_t _index;
};

}

#endif
