#include "api/vulkan/vulkan_pipeline_layout.h"

using namespace rend;

VulkanPipelineLayout::VulkanPipelineLayout(const std::string& name, VkPipelineLayout vk_handle)
    :
        PipelineLayout(name),
        _vk_handle(vk_handle)
{
}

VkPipelineLayout VulkanPipelineLayout::vk_handle(void) const
{
    return _vk_handle;
}
