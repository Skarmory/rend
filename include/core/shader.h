#ifndef REND_SHADER_H
#define REND_SHADER_H

#include "core/rend_defs.h"

#include <cstddef>
#include <cstdint>

namespace rend
{

class Shader
{
public:
    Shader(const void* code, uint32_t size_bytes, ShaderStage type);
    ~Shader(void);
    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    ShaderHandle handle(void) const;
    ShaderStage  type(void) const;

private:
    ShaderHandle _handle{ NULL_HANDLE };
    size_t       _bytes{ 0 };
    ShaderStage  _type{ ShaderStage::SHADER_STAGE_NONE };
};

}

#endif
