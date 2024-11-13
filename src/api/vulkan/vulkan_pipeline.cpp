#include "api/vulkan/vulkan_pipeline.h"

using namespace rend;

VulkanPipeline::VulkanPipeline(const std::string& name, const PipelineInfo& info, VkPipeline vk_handle)
    :
        Pipeline(name, info),
        _vk_handle(vk_handle)
{
}

VkPipeline VulkanPipeline::vk_handle(void) const
{
    return _vk_handle;
}
