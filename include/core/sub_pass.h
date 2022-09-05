#ifndef REND_CORE_SUB_PASS_H
#define REND_CORE_SUB_PASS_H

#include "core/rend_defs.h"
#include "core/gpu_resource.h"

#include <string>
#include <vector>

namespace rend
{

class CommandBuffer;

class SubPass : public GPUResource
{
    public:
        SubPass(const std::string& name, ShaderSetHandle shader_set, RenderPassHandle render_pass, uint32_t subpass_idx);
        ~SubPass(void);

        ShaderSetHandle get_shader_set(void) const;
        uint32_t        get_index(void) const;

        void begin(CommandBuffer& command_buffer);

    private:
        PipelineHandle _pipeline_handle{ NULL_HANDLE };
        ShaderSetHandle _shader_set_handle { NULL_HANDLE };
        uint32_t _subpass_idx;
};

}

#endif
