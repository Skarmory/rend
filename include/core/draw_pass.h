#ifndef REND_CORE_DRAW_PASS_H
#define REND_CORE_DRAW_PASS_H

#include <stdint.h>
#include <string>
#include <vector>

//#include "core/draw_item.h"
#include "core/gpu_resource.h"
//#include "core/rend_object.h"
#include "core/sub_pass.h"

namespace rend
{

class CommandBuffer;
class Pipeline;
class RenderPass;
class ShaderSet;
class SubPass;
struct PerPassData;
struct SubPassInfo;

struct DrawPassInfo
{
    std::string named_framebuffer;
    RenderPass* render_pass{ nullptr };
    std::vector<Pipeline*> subpass_pipelines;
    ColourClear colour_clear;
    DepthStencilClear depth_stencil_clear;
};

class DrawPass : public GPUResource//, public RendObject
{
    public:
        DrawPass(const std::string& name, const DrawPassInfo& info);
        ~DrawPass(void);

        std::vector<SubPass>& get_subpasses(void);
        const std::string& get_named_framebuffer(void) const;
        const ColourClear& get_colour_clear(void) const;
        const DepthStencilClear& get_depth_stencil_clear(void) const;

        bool has_next_subpass(void) const;
        void add_subpass(const std::string& name, const SubPassInfo& info);

        void begin(CommandBuffer& command_buffer, const PerPassData& per_pass);
        void end(CommandBuffer& command_buffer);
        void next_subpass(CommandBuffer& command_buffer);

    private:
        RenderPass& _render_pass;
        std::vector<SubPass> _subpasses;
        uint32_t _current_subpass{ 0 };
        ColourClear _colour_clear{};
        DepthStencilClear _depth_stencil_clear{};
        std::string _named_framebuffer;
};

}

#endif
