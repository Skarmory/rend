#include "core/render_pass.h"

#include "core/command_buffer.h"
#include "core/device_context.h"
#include "core/draw_item.h"
#include "core/framebuffer.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/rend_service.h"

#include <cassert>
#include <functional>

using namespace rend;

RenderPass::RenderPass(const std::string& name, const RenderPassInfo& info)
    :   GPUResource(name),
        _info(info)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_render_pass(_info);

    auto* rr = RendService::renderer();

    for(int i = 0; i < _info.subpasses_count; ++i)
    {
        std::string subpass_name = name + ", Subpass: " + std::to_string(i);
        _subpasses.emplace_back(
            rr->create_sub_pass(subpass_name, _info.subpasses[i].shader_set_handle, _handle, i)
        );
    }
}

RenderPass::~RenderPass(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_render_pass(_handle);
}

RenderPassHandle RenderPass::handle(void) const
{
    return _handle;
}

std::vector<SubPassHandle> RenderPass::get_subpasses(void) const
{
    return _subpasses;
}

bool RenderPass::has_next_subpass(void) const
{
    return _current_subpass + 1 != _subpasses.size();
}

void RenderPass::begin(CommandBuffer& command_buffer, PerPassData& per_pass)
{
    auto* rr = RendService::renderer();
    Framebuffer* fb = rr->get_framebuffer(per_pass.framebuffer);

    command_buffer.begin_render_pass(*this, *fb, per_pass.render_area, per_pass.colour_clear, per_pass.depth_clear);

    SubPass* sp = rr->get_sub_pass(_subpasses[_current_subpass]);
    sp->begin(command_buffer);
}

void RenderPass::next_subpass(CommandBuffer& command_buffer)
{
    if(has_next_subpass())
    {
        auto* rr = RendService::renderer();
        _current_subpass++;
        command_buffer.next_subpass();
        SubPass* sp = rr->get_sub_pass(_subpasses[_current_subpass]);
        sp->begin(command_buffer);
    }
}

void RenderPass::end(CommandBuffer& command_buffer)
{
    command_buffer.end_render_pass();
    _current_subpass = 0;
}
