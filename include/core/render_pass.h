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
struct DrawItem;
struct PerPassData;

struct SubPassDescription
{
    PipelineBindPoint bind_point{ PipelineBindPoint::GRAPHICS };
    uint32_t          colour_attachment_infos[rend::constants::max_framebuffer_attachments];
    uint32_t          input_attachment_infos[rend::constants::max_framebuffer_attachments];
    uint32_t          resolve_attachment_infos[rend::constants::max_framebuffer_attachments];
    uint32_t          depth_stencil_attachment{};
    uint32_t          preserve_attachments[rend::constants::max_framebuffer_attachments];
    uint32_t          colour_attachment_infos_count{ 0 };
    uint32_t          input_attachment_infos_count{ 0 };
    uint32_t          resolve_attachment_infos_count{ 0 };
    uint32_t          preserve_attachments_count{ 0 };
    const ShaderSet*  shader_set{ nullptr };
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
    AttachmentInfo     attachment_infos[rend::constants::max_framebuffer_attachments];
    SubPassDescription subpasses[rend::constants::max_subpasses];
    SubPassDependency  subpass_dependencies[rend::constants::max_subpasses];
    uint32_t           attachment_infos_count{ 0 };
    uint32_t           subpasses_count{ 0 };
    uint32_t           subpass_dependency_count{ 0 };
};

class RenderPass : public GPUResource, public RendObject
{
public:
    explicit RenderPass(const std::string& name, const RenderPassInfo& info, RendHandle rend_handle);
    virtual ~RenderPass(void) = default;
    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    virtual const std::vector<SubPass*>& get_subpasses(void) const = 0;
    virtual bool has_next_subpass(void) const = 0;

    virtual void add_subpass(const std::string& name, const SubPassInfo& info) = 0;

    virtual void begin(CommandBuffer& command_buffer, PerPassData& per_pass) = 0;
    virtual void next_subpass(CommandBuffer& command_buffer) = 0;
    virtual void end(CommandBuffer& command_buffer) = 0;

protected:
    std::vector<SubPass*> _subpasses;
    uint32_t              _current_subpass{ 0 };

private:
    RenderPassInfo _info{};
};

}

#endif
