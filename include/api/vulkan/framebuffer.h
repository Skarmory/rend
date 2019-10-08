#ifndef REND_FRAME_BUFFER_H
#define REND_FRAME_BUFFER_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class DeviceContext;
class RenderPass;

class Framebuffer
{
public:
    explicit Framebuffer(DeviceContext& device);
    ~Framebuffer(void);

    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    bool create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    bool recreate(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);

    VkFramebuffer                  get_handle(void) const;
    const VkFramebufferCreateInfo& get_vk_create_info(void) const;
    const RenderPass*              get_render_pass(void) const;

private:
    bool _create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    void _destroy(void);

private:
    DeviceContext&          _context;
    const RenderPass*       _render_pass;

    VkFramebufferCreateInfo _vk_create_info;
    VkFramebuffer           _vk_framebuffer;
};

}

#endif
