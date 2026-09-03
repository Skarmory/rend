#ifndef REND_CORE_SHADER_H
#define REND_CORE_SHADER_H

#include "rend/rend_api.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_defs.h"
#include "rend/core/rend_object.h"

#include <string>

namespace rend
{

class REND_API Shader : public GPUResource, public RendObject
{
public:
    Shader(const std::string& name, size_t size_bytes, ShaderStage type);
    virtual ~Shader(void) = default;
    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    ShaderStage     type(void) const;

private:
    size_t      _bytes{ 0 };
    ShaderStage _type{ ShaderStage::SHADER_STAGE_NONE };
};

}

#endif
