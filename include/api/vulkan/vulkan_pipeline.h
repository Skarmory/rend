#ifndef REND_API_VULKAN_VULKAN_PIPELINE_H
#define REND_API_VULKAN_VULKAN_PIPELINE_H

#include "core/pipeline.h"
#include <vulkan.h>

namespace rend
{

class VulkanPipeline : public Pipeline
{
public:
    VulkanPipeline(const std::string& name, const PipelineInfo& info, RendHandle rend_handle, VkPipeline vk_handle);
    ~VulkanPipeline(void) = default;

    VkPipeline vk_handle(void) const;

private:
    VkPipeline _vk_handle{ VK_NULL_HANDLE };
};

}

#endif
