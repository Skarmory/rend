#include "core/sub_pass.h"

#include "core/command_buffer.h"
#include "core/logging/log_defs.h"
#include "core/logging/log_manager.h"

using namespace rend;

SubPass::SubPass(const std::string& name, const SubPassInfo& info)
    :
        GPUResource(name),
        _info(info)
{
}

Pipeline& SubPass::get_pipeline(void)
{
    return *_info.pipeline;
}

//const ShaderSet& SubPass::get_shader_set(void) const
//{
//    return *_info.shader_set;
//}

uint32_t SubPass::get_index(void) const
{
    return _info.subpass_index;
}

void SubPass::begin(CommandBuffer& command_buffer)
{
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "RENDERER | Begin sub pass: " + name());
    command_buffer.bind_pipeline(PipelineBindPoint::GRAPHICS, *_info.pipeline);
}
