#include "core/sub_pass.h"

#include "core/command_buffer.h"

using namespace rend;

SubPass::SubPass(const std::string& name, const SubPassInfo& info, Pipeline* pipeline, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _pipeline(pipeline),
        _info(info)
{
}

const Pipeline& SubPass::get_pipeline(void) const
{
    return *_pipeline;
}

const ShaderSet& SubPass::get_shader_set(void) const
{
    return *_info.shader_set;
}

uint32_t SubPass::get_index(void) const
{
    return _info.subpass_index;
}

void SubPass::begin(CommandBuffer& command_buffer)
{
    command_buffer.bind_pipeline(PipelineBindPoint::GRAPHICS, *_pipeline);
}
