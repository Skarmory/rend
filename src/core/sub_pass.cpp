#include "core/sub_pass.h"

#include "core/command_buffer.h"
#include "core/rend_service.h"

#include "api/vulkan/renderer.h"

using namespace rend;

SubPass::SubPass(const std::string& name, ShaderSetHandle shader_set, RenderPassHandle render_pass, uint32_t subpass_idx)
    :
        GPUResource(name),
        _shader_set_handle(shader_set),
        _subpass_idx(subpass_idx)
{
    auto* rr = RendService::renderer();
    auto* ss = rr->get_shader_set(shader_set);

    PipelineInfo pl_info{};

    pl_info.shaders[0] = ss->get_shader(ShaderIndex::SHADER_INDEX_VERTEX);
    pl_info.shaders[1] = ss->get_shader(ShaderIndex::SHADER_INDEX_FRAGMENT);
    pl_info.vertex_binding_info.index = 0;
    pl_info.vertex_binding_info.stride = 32; // TODO: Figure out how to work out vertex bindings properly

    const auto& va_infos = ss->get_vertex_attribute_infos();
    pl_info.vertex_attribute_info_count = va_infos.size();

    for(size_t i = 0; i < va_infos.size(); ++i)
    {
        pl_info.vertex_attribute_infos[i] = va_infos[i];
        pl_info.vertex_attribute_infos[i].binding = &pl_info.vertex_binding_info;
    }

    // TODO: Make this dynamic
    pl_info.colour_blending_info.blend_attachments[0].blend_enabled = true;
    pl_info.colour_blending_info.blend_attachments[0].colour_src_factor = BlendFactor::ONE;
    pl_info.colour_blending_info.blend_attachments[0].colour_dst_factor = BlendFactor::ZERO;
    pl_info.colour_blending_info.blend_attachments_count = 1;

    pl_info.dynamic_states = DynamicState::VIEWPORT | DynamicState::SCISSOR;
    pl_info.viewport_info_count = 1;
    pl_info.scissor_info_count = 1;

    pl_info.layout_handle = ss->get_pipeline_layout();
    pl_info.render_pass_handle = render_pass;

    _pipeline_handle = rr->create_pipeline(name + " pipeline", pl_info);
}

SubPass::~SubPass(void)
{
    auto* rr = RendService::renderer();
    rr->destroy_pipeline(_pipeline_handle);
}

ShaderSetHandle SubPass::get_shader_set(void) const
{
    return _shader_set_handle;
}

uint32_t SubPass::get_index(void) const
{
    return _subpass_idx;
}

void SubPass::begin(CommandBuffer& command_buffer)
{
    auto* rr = RendService::renderer();
    Pipeline* pl = rr->get_pipeline(_pipeline_handle);

    command_buffer.bind_pipeline(PipelineBindPoint::GRAPHICS, *pl);
}
