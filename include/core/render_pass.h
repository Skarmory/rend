#ifndef REND_CORE_RENDER_PASS_H
#define REND_CORE_RENDER_PASS_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"
#include "core/sub_pass.h"
#include <string>
#include <vector>

namespace rend
{

class CommandBuffer;
class GPUTexture;
struct DrawItem;
struct PerPassData;

struct SubPassDescription
{
    PipelineBindPoint bind_point{ PipelineBindPoint::GRAPHICS };
    std::vector<uint32_t> colour_attachment_infos;
    std::vector<uint32_t> input_attachment_infos;
    std::vector<uint32_t> resolve_attachment_infos;
    std::vector<uint32_t> preserve_attachments;
    uint32_t              depth_stencil_attachment{};
};

struct SubPassDependency
{
    uint32_t        src_subpass{ 0 };
    uint32_t        dst_subpass{ 0 };
    Synchronisation src_sync{};
    Synchronisation dst_sync{};
};

struct RenderPassInfo
{
    std::vector<AttachmentInfo>     attachment_infos;
    std::vector<SubPassDescription> subpasses;
    std::vector<SubPassDependency>  subpass_dependencies;
};

class RenderPass : public GPUResource, public RendObject
{
public:
    RenderPass(const std::string& name, const RenderPassInfo& info);
    virtual ~RenderPass(void) = default;
    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    const RenderPassInfo& get_info(void) const;
    bool has_depth_target(void) const;
    uint32_t colour_targets_count(void) const;

    virtual void begin(CommandBuffer& command_buffer, const PerPassData& per_pass) = 0;
    virtual void next_subpass(CommandBuffer& command_buffer) = 0;
    virtual void end(CommandBuffer& command_buffer) = 0;

private:
    RenderPassInfo _info{};
    bool _has_depth_target{ false };
    uint32_t _colour_targets_count{ 0 };
};

}

#endif
