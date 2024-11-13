#ifndef REND_CORE_RENDER_STRATEGY_H
#define REND_CORE_RENDER_STRATEGY_H

#include <string>
#include <vector>

#include "core/descriptor_frequency.h"
#include "core/draw_pass.h"
#include "core/gpu_resource.h"
#include "core/rend_object.h"

namespace rend
{

class DescriptorSetLayout;

struct RenderStrategyInfo
{
    std::vector<DrawPassInfo> draw_passes;
};

class RenderStrategy : public GPUResource, public RendObject
{
    public:
        RenderStrategy(const std::string& name, const RenderStrategyInfo& info);
        ~RenderStrategy(void);

        std::vector<DrawPass>& get_draw_passes(void);
        const DescriptorSetLayout& get_descriptor_set_layout(DescriptorFrequency freq);

    private:
        std::vector<DrawPass> _draw_passes;
};

}

#endif
