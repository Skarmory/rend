#include "core/render_strategy.h"

#include "core/descriptor_set_layout.h"
#include "core/pipeline.h"
#include "core/renderer.h"

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

const DescriptorSetLayout& RenderStrategy::get_descriptor_set_layout(DescriptorFrequency freq)
{
    auto& pipeline = _draw_passes[0].get_subpasses()[0].get_pipeline();
    auto& descriptor_set_layout = pipeline.get_shader_set().get_descriptor_set_layout(freq);
    return descriptor_set_layout;
}
