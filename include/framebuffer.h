#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <vulkan/vulkan.h>
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

    VkFramebuffer get_handle(void) const;

private:
    Framebuffer(LogicalDevice* device, const RenderPass& render_pass, const std::vector<VkImageView>& attachments, VkExtent3D dimensions);
    ~Framebuffer(void);

private:
    VkFramebuffer _vk_framebuffer;

    LogicalDevice* _device;
};

}

#endif
