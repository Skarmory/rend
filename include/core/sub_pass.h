#ifndef REND_CORE_SUB_PASS_H
#define REND_CORE_SUB_PASS_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <string>
#include <vector>

namespace rend
{

class CommandBuffer;
class Pipeline;
class RenderPass;
class ShaderSet;

struct SubPassInfo
{
    RenderPass* render_pass;
    uint32_t    subpass_index;
    const ShaderSet* shader_set;
};

class SubPass : public GPUResource, public RendObject
{
    public:
        SubPass(const std::string& name, const SubPassInfo& info, Pipeline* pipeline, RendHandle rend_handle);
        ~SubPass(void) = default;

        [[nodiscard]] const Pipeline&  get_pipeline(void) const;
        [[nodiscard]] const ShaderSet& get_shader_set(void) const;
        uint32_t   get_index(void) const;

        void begin(CommandBuffer& command_buffer);

    private:
        Pipeline*   _pipeline{ nullptr };
        SubPassInfo _info{};
};

}

#endif
