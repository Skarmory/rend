#include "rend/core/render_strategy.h"

#include "rend/core/descriptor_set_layout.h"
#include "rend/core/pipeline.h"
#include "rend/core/renderer.h"

using namespace rend;

RenderStrategy::RenderStrategy(const std::string& name, const RenderStrategyInfo& info)
    :
        GPUResource(name)
{
    for(int i = 0; i < info.draw_passes.size(); ++i)
    {
        _draw_passes.emplace_back(name + ", draw pass #" + std::to_string(i), info.draw_passes[i]);
    }
}

RenderStrategy::~RenderStrategy(void)
{
    _draw_passes.clear();
    //for(int i = 0; i < _draw_passes.size(); ++i)
    //{
    //    rr.destroy_draw_pass(_draw_passes[i]);
    //}
}

std::vector<DrawPass>& RenderStrategy::get_draw_passes(void)
{
    return _draw_passes;
}

const DescriptorSetLayout& RenderStrategy::get_descriptor_set_layout(DescriptorUpdateRate update_rate)
{
    auto& pipeline = _draw_passes[0].get_subpasses()[0].get_pipeline();
    auto& descriptor_set_layout = pipeline.get_shader_set().get_descriptor_set_layout(update_rate);
    return descriptor_set_layout;
}
