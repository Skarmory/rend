#include "api/vulkan/vulkan_pipeline_layout.h"

using namespace rend;

VulkanPipelineLayout::VulkanPipelineLayout(const std::string& name, RendHandle rend_handle, VkPipelineLayout vk_handle)
    :
        PipelineLayout(name, rend_handle),
        _vk_handle(vk_handle)
{
}

VkPipelineLayout VulkanPipelineLayout::vk_handle(void) const
{
    return _vk_handle;
}
