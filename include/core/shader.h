#ifndef REND_SHADER_H
#define REND_SHADER_H

#include "rend_defs.h"

#include <cstddef>
#include <cstdint>

namespace rend
{

class Shader
{
public:
    Shader(void)                     = default;
    ~Shader(void)                    = default;
    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    bool create(const void* code, uint32_t size_bytes, ShaderStage type);
    void destroy(void);

    ShaderHandle get_handle(void) const;
    ShaderStage   get_type(void) const;

private:
    ShaderHandle _handle{ NULL_HANDLE };
    size_t       _bytes{ 0 };
    ShaderStage  _type{ ShaderStage::SHADER_STAGE_NONE };
};

}

#endif
