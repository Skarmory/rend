#ifndef REND_API_VULKAN_VULKAN_PIPELINE_LAYOUT_H
#define REND_API_VULKAN_VULKAN_PIPELINE_LAYOUT_H

#include "rend/core/pipeline_layout.h"
#include <vulkan/vulkan.h>

namespace rend
{

class VulkanPipelineLayout : public PipelineLayout
{
public:
    VulkanPipelineLayout(const std::string& name, VkPipelineLayout vk_handle);
    ~VulkanPipelineLayout(void) = default;

    VkPipelineLayout vk_handle(void) const;

private:
    VkPipelineLayout _vk_handle{ VK_NULL_HANDLE };
};

}

#endif
