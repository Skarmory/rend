#ifndef REND_SHADER_BASE_H
#define REND_SHADER_BASE_H

#include "gpu_resource.h"
#include "rend_defs.h"

namespace rend
{

class ShaderBase : public GPUResource
{
public:
    ShaderBase(void);
    virtual ~ShaderBase(void);

    ShaderBase(const ShaderBase&)            = delete;
    ShaderBase(ShaderBase&&)                 = delete;
    ShaderBase& operator=(const ShaderBase&) = delete;
    ShaderBase& operator=(ShaderBase&&)      = delete;

    ShaderType shader_type(void) const;

protected:
    ShaderType _type;
};

}

#endif
