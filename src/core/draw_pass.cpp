#include "core/draw_pass.h"

#include "core/renderer.h"
#include "core/render_pass.h"
#include "core/sub_pass.h"

#include "core/logging/log_defs.h"
#include "core/logging/log_helper_funcs.h"
#include "core/logging/log_manager.h"

//#include <sstream>

using namespace rend;

DrawPass::DrawPass(const std::string& name, const DrawPassInfo& info)
    :
        GPUResource(name),
        _render_pass(*info.render_pass),
        _colour_clear(info.colour_clear),
        _depth_stencil_clear(info.depth_stencil_clear),
        _named_framebuffer(info.named_framebuffer)
{
    //auto& rr = Renderer::get_instance();

    for(size_t i = 0; i < info.subpass_pipelines.size(); ++i)
    {
        //std::stringstream ss;
        //ss << name << ", subpass #" << i;

        SubPassInfo sp_info =
        {
            .pipeline = info.subpass_pipelines[i],
            .subpass_index = (uint32_t)i
        };

        //auto* subpass = rr.create_sub_pass(ss.str(), sp_info);
        _subpasses.emplace_back(name + ", subpass #" + std::to_string(i), sp_info);
    }
}

DrawPass::~DrawPass(void)
{
    _subpasses.clear();

    //auto& rr = Renderer::get_instance();
    //for(int i = 0; i < _subpasses.size(); ++i)
    //{
    //    //rr.destroy_sub_pass(_subpasses[i]);
    //}
}

const std::string& DrawPass::get_named_framebuffer(void) const
{
    return  _named_framebuffer;
}

const ColourClear& DrawPass::get_colour_clear(void) const
{
    return _colour_clear;
}

const DepthStencilClear& DrawPass::get_depth_stencil_clear(void) const
{
    return _depth_stencil_clear;
}

bool DrawPass::has_next_subpass(void) const
{
    return _current_subpass + 1 != _subpasses.size();
}

void DrawPass::add_subpass(const std::string& name, const SubPassInfo& info)
{
    //auto* subpass = Renderer::get_instance().create_sub_pass(name, info);
    _subpasses.emplace_back(this->name() + " " + name, info);
}

std::vector<SubPass>& DrawPass::get_subpasses(void)
{
    return _subpasses;
}

void DrawPass::begin(CommandBuffer& command_buffer, const PerPassData& per_pass_data)
{
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "RENDERER | Begin draw pass (" + name() + ") with params: " + core::logging::to_string(per_pass_data));

    _current_subpass = 0;
    _render_pass.begin(command_buffer, per_pass_data);
    _subpasses[_current_subpass].begin(command_buffer);
}

void DrawPass::end(CommandBuffer& command_buffer)
{
    _render_pass.end(command_buffer);
}

void DrawPass::next_subpass(CommandBuffer& command_buffer)
{
    if(has_next_subpass())
    {
        ++_current_subpass;
        _render_pass.next_subpass(command_buffer);
        _subpasses[_current_subpass].begin(command_buffer);
    }
}
