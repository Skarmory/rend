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
//    const RenderPass* render_pass{ nullptr };
//    const ShaderSet*  shader_set{ nullptr };
    Pipeline*   pipeline{ nullptr };
    uint32_t          subpass_index{ 0 };
};

class SubPass : public GPUResource//, public RendObject
{
    friend class DrawPass;

    public:
        SubPass(const std::string& name, const SubPassInfo& info);
        ~SubPass(void) = default;

        [[nodiscard]] Pipeline&  get_pipeline(void);
        //[[nodiscard]] const ShaderSet& get_shader_set(void) const;
        uint32_t   get_index(void) const;

        void begin(CommandBuffer& command_buffer);

    private:
        SubPassInfo _info{};
};

}

#endif
