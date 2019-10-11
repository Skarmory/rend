#ifndef REND_FRAME_BUFFER_H
#define REND_FRAME_BUFFER_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class DepthBuffer;
class DeviceContext;
class RenderPass;
class RenderTarget;

class Framebuffer
{
public:
    explicit Framebuffer(DeviceContext& device);
    ~Framebuffer(void);

    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    // These two are for internal use only right now
    bool create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    bool recreate(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);

    bool create_framebuffer(const RenderPass& render_pass, VkExtent3D dimensions);
    bool recreate(VkExtent3D dimensions);

    bool add_render_target(RenderTarget& target);
    bool set_depth_buffer(DepthBuffer& buffer);

    VkFramebuffer                     get_handle(void) const;
    const VkFramebufferCreateInfo&    get_vk_create_info(void) const;
    const RenderPass*                 get_render_pass(void) const;
    const std::vector<RenderTarget*>& get_render_targets(void) const;
    const DepthBuffer*                get_depth_buffer(void) const;

private:
    bool _create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    void _destroy(void);

private:
    DeviceContext&          _context;
    const RenderPass*       _render_pass;

    std::vector<RenderTarget*> _render_targets;
    DepthBuffer*               _depth_buffer;

    VkFramebufferCreateInfo _vk_create_info;
    VkFramebuffer           _vk_framebuffer;
};

}

#endif
