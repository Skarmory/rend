#ifndef REND_CORE_RENDER_STRATEGY_H
#define REND_CORE_RENDER_STRATEGY_H

#include "rend/rend_api.h"
#include "rend/core/descriptor_update_rate.h"
#include "rend/core/draw_pass.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

class DescriptorSetLayout;

struct RenderStrategyInfo
{
    std::vector<DrawPassInfo> draw_passes;
};

class REND_API RenderStrategy : public GPUResource, public RendObject
{
    public:
        RenderStrategy(const std::string& name, const RenderStrategyInfo& info);
        ~RenderStrategy(void);

        RenderStrategy(const RenderStrategy&) = delete;
        RenderStrategy& operator=(const RenderStrategy&) = delete;

        std::vector<DrawPass>& get_draw_passes(void);
        const DescriptorSetLayout& get_descriptor_set_layout(DescriptorUpdateRate update_rate);

    private:
        std::vector<DrawPass> _draw_passes;
};

}

#endif
