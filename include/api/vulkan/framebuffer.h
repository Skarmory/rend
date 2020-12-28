#ifndef REND_FRAME_BUFFER_H
#define REND_FRAME_BUFFER_H

#include "rend_defs.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class GPUTexture;
class RenderPass;

class Framebuffer
{
public:
    Framebuffer(void) = default;
    ~Framebuffer(void) = default;

    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    StatusCode create_framebuffer(const RenderPass& render_pass, VkExtent3D dimensions);
    void destroy(void);

    bool add_render_target(Texture2DHandle target);
    bool set_depth_buffer(GPUTexture& buffer);

    VkFramebuffer     get_handle(void) const;
    const RenderPass* get_render_pass(void) const;
    const GPUTexture* get_depth_buffer(void) const;

    void on_end_render_pass(void);

private:
    StatusCode _create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    void _destroy(void);

private:
    const RenderPass*            _render_pass { nullptr };
    GPUTexture*                  _depth_buffer { nullptr };
    std::vector<Texture2DHandle> _render_targets;
    VkFramebuffer                _vk_framebuffer { VK_NULL_HANDLE };
};

}

#endif
