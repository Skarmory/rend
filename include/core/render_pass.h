#ifndef REND_RENDERPASS_H
#define REND_RENDERPASS_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/sub_pass.h"

#include <string>
#include <vector>

namespace rend
{

class CommandBuffer;
struct DrawItem;
struct PerPassData;

class RenderPass : public GPUResource
{
public:
    explicit RenderPass(const std::string& name, const RenderPassInfo& info);
    ~RenderPass(void);
    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    RenderPassHandle handle(void) const;
    std::vector<SubPassHandle> get_subpasses(void) const;
    bool has_next_subpass(void) const;

    void begin(CommandBuffer& command_buffer, PerPassData& per_pass);
    void next_subpass(CommandBuffer& command_buffer);
    void end(CommandBuffer& command_buffer);


private:
    RenderPassHandle           _handle{ NULL_HANDLE };
    RenderPassInfo             _info{};
    std::vector<SubPassHandle> _subpasses;
    uint32_t                   _current_subpass{ 0 };
};

}

#endif

