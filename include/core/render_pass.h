#ifndef REND_RENDERPASS_H
#define REND_RENDERPASS_H

#include "core/rend_defs.h"

namespace rend
{

class RenderPass
{
public:
    explicit RenderPass(const RenderPassInfo& info);
    ~RenderPass(void);
    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    RenderPassHandle handle(void) const;

private:
    RenderPassHandle _handle{ NULL_HANDLE };
    RenderPassInfo   _info{};
};

}

#endif

