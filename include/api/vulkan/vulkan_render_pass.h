#ifndef REND_API_VULKAN_VULKAN_RENDER_PASS_H
#define REND_API_VULKAN_VULKAN_RENDER_PASS_H

#include "core/render_pass.h"

#include <string>
#include <vector>
#include <vulkan.h>

namespace rend
{

class GPUTexture;
class VulkanSubPass;

class VulkanRenderPass : public RenderPass 
{
    friend class VulkanRenderer;

public:
    VulkanRenderPass(const std::string& name, const RenderPassInfo& info, VkRenderPass vk_handle);
    ~VulkanRenderPass(void);

    VkRenderPass vk_handle(void) const;

    void begin(CommandBuffer& command_buffer, const PerPassData& per_pass_data) override;
    void end(CommandBuffer& command_buffer) override;
    void next_subpass(CommandBuffer& command_buffer) override;

private:
    VkRenderPass _vk_handle{ VK_NULL_HANDLE };
};

}

#endif

