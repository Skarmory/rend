#ifndef REND_API_VULKAN_VULKAN_RENDER_PASS_H
#define REND_API_VULKAN_VULKAN_RENDER_PASS_H

#include "core/render_pass.h"

#include <string>
#include <vector>
#include <vulkan.h>

namespace rend
{

class VulkanSubPass;

class VulkanRenderPass : public RenderPass 
{
    friend class VulkanRenderer;

public:
    VulkanRenderPass(const std::string& name, const RenderPassInfo& info, VkRenderPass vk_handle, RendHandle rend_handle);
    ~VulkanRenderPass(void);

    const std::vector<SubPass*>& get_subpasses(void) const override;
    bool has_next_subpass(void) const override;
    VkRenderPass vk_handle(void) const;

    void add_subpass(const std::string& name, const SubPassInfo& info) override;

    void begin(CommandBuffer& command_buffer, PerPassData& per_pass) override;
    void next_subpass(CommandBuffer& command_buffer) override;
    void end(CommandBuffer& command_buffer) override;

private:
    VkRenderPass _vk_handle{ VK_NULL_HANDLE };
    uint32_t     _current_subpass{ 0 };
};

}

#endif

