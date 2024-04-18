#ifndef REND_CORE_SHADER_H
#define REND_CORE_SHADER_H

#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

namespace rend
{

class Shader : public GPUResource, public RendObject
{
public:
    Shader(const std::string& name, size_t size_bytes, ShaderStage type, RendHandle rend_handle);
    virtual ~Shader(void) = default;
    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    ShaderStage     type(void) const;

private:
    size_t          _bytes{ 0 };
    ShaderStage     _type{ ShaderStage::SHADER_STAGE_NONE };
};

}

#endif
