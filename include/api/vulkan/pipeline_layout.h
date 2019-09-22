#ifndef PIPELINE_LAYOUT_H
#define PIPELINE_LAYOUT_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class DescriptorSetLayout;
class LogicalDevice;

class PipelineLayout
{
    friend class LogicalDevice;

public:
    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout(PipelineLayout&&)      = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&)      = delete;

    VkPipelineLayout get_handle(void) const;

private:
    PipelineLayout(LogicalDevice* device, const std::vector<DescriptorSetLayout*>& desc_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges);
    ~PipelineLayout(void);

private:
    VkPipelineLayout _vk_layout;

    LogicalDevice* _device;
};

}

#endif
