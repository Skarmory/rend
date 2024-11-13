#include "core/render_pass.h"

#include "core/rend_utils.h"
#include "core/logging/log_defs.h"
#include "core/logging/log_helper_funcs.h"
#include "core/logging/log_manager.h"

using namespace rend;

RenderPass::RenderPass(const std::string& name, const RenderPassInfo& info)
    :
        GPUResource(name),
        _info(info)
{
#ifdef DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "RENDER PASS | Created render pass with params: " + rend::core::logging::to_string(_info));
#endif

    for(auto& attachment_info : _info.attachment_infos)
    {
        if(is_depth_format(attachment_info.format))
        {
            _has_depth_target = true;
        }
        else
        {
            ++_colour_targets_count;
        }
    }
}

const RenderPassInfo& RenderPass::get_info(void) const
{
    return _info;
}

bool RenderPass::has_depth_target(void) const
{
    return _has_depth_target;
}

uint32_t RenderPass::colour_targets_count(void) const
{
    return _colour_targets_count;
}
