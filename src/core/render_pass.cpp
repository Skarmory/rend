#include "core/render_pass.h"

using namespace rend;

RenderPass::RenderPass(const std::string& name, const RenderPassInfo& info, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _info(info)
{
}
