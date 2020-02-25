#ifndef REND_FRAME_BUFFER_H
#define REND_FRAME_BUFFER_H

#include "rend_defs.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class DepthBuffer;
class RenderPass;
class RenderTarget;

class Framebuffer
{
public:
    Framebuffer(void) = default;
    ~Framebuffer(void);

    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    StatusCode create_framebuffer(const RenderPass& render_pass, VkExtent3D dimensions);
    StatusCode recreate(VkExtent3D dimensions);

    bool add_render_target(RenderTarget& target);
    bool set_depth_buffer(DepthBuffer& buffer);

    VkFramebuffer                     get_handle(void) const;
    const RenderPass*                 get_render_pass(void) const;
    const std::vector<RenderTarget*>& get_render_targets(void) const;
    const DepthBuffer*                get_depth_buffer(void) const;

    void on_end_render_pass(void);

private:
    StatusCode _create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    void _destroy(void);

private:
    const RenderPass*          _render_pass { nullptr };
    std::vector<RenderTarget*> _render_targets;
    DepthBuffer*               _depth_buffer { nullptr };
    VkFramebuffer              _vk_framebuffer { VK_NULL_HANDLE };
};

}

#endif
