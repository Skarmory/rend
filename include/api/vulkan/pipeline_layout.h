#ifndef REND_PIPELINE_LAYOUT_H
#define REND_PIPELINE_LAYOUT_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class DescriptorSetLayout;
class DeviceContext;

class PipelineLayout
{
public:
    PipelineLayout(DeviceContext* context);
    ~PipelineLayout(void);
    PipelineLayout(const PipelineLayout&)            = delete;
    PipelineLayout(PipelineLayout&&)                 = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&)      = delete;

    bool create_pipeline_layout(const std::vector<DescriptorSetLayout*>& desc_set_layouts, std::vector<VkPushConstantRange>& push_constant_ranges);

    VkPipelineLayout get_handle(void) const;

private:
    DeviceContext* _context;

    VkPipelineLayout _vk_layout;
};

}

#endif
