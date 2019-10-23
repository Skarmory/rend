#ifndef REND_PIPELINE_LAYOUT_H
#define REND_PIPELINE_LAYOUT_H

#include "rend_defs.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class DescriptorSetLayout;
class DeviceContext;

class PipelineLayout
{
public:
    explicit PipelineLayout(DeviceContext& context);
    ~PipelineLayout(void);

    PipelineLayout(const PipelineLayout&)            = delete;
    PipelineLayout(PipelineLayout&&)                 = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&)      = delete;

    StatusCode create_pipeline_layout(void);

    void add_push_constant_range(ShaderType type, uint32_t offset, uint32_t size_bytes);
    void add_descriptor_set_layout(DescriptorSetLayout& layout);

    VkPipelineLayout get_handle(void) const;

private:
    DeviceContext& _context;
    std::vector<VkPushConstantRange> _push_constant_ranges;
    std::vector<DescriptorSetLayout*> _descriptor_set_layouts;

    VkPipelineLayout _vk_layout;
};

}

#endif
