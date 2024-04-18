#include "api/vulkan/vulkan_pipeline.h"

using namespace rend;

VulkanPipeline::VulkanPipeline(const std::string& name, const PipelineInfo& info, RendHandle rend_handle, VkPipeline vk_handle)
    :
        Pipeline(name, info, rend_handle),
        _vk_handle(vk_handle)
{
}

VkPipeline VulkanPipeline::vk_handle(void) const
{
    return _vk_handle;
}
