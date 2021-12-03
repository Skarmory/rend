#ifndef REND_RENDERPASS_H
#define REND_RENDERPASS_H

#include "core/rend_defs.h"

namespace rend
{

class RenderPass
{
public:
    RenderPass(void)                         = default;
    ~RenderPass(void)                        = default;
    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    StatusCode create(const RenderPassInfo& info);
    void       destroy(void);

    RenderPassHandle handle(void) const;

private:
    RenderPassHandle _handle{ NULL_HANDLE };
    RenderPassInfo   _render_pass_info{};
};

}

#endif

