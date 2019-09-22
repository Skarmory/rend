#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;
class RenderPass;

class Framebuffer
{
    friend class LogicalDevice;

public:

    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer(Framebuffer&&)                 = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&)      = delete;

    VkFramebuffer       get_handle(void) const;
    const RenderPass*   get_render_pass(void) const;

    void recreate(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);

private:
    Framebuffer(LogicalDevice* device, const RenderPass& render_pass, const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    ~Framebuffer(void);

    void _create(const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    void _destroy(void);

private:
    VkFramebufferCreateInfo _vk_create_info;
    VkFramebuffer           _vk_framebuffer;

    LogicalDevice*    _device;
    const RenderPass* _render_pass;
};

}

#endif
