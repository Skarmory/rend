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

    bool create(const void* code, uint32_t size_bytes, ShaderType type);
    void destroy(void);

    ShaderHandle get_handle(void) const;
    ShaderType   get_type(void) const;

private:
    ShaderHandle _handle{ NULL_HANDLE };
    size_t       _bytes{ 0 };
    ShaderType   _type{ ShaderType::VERTEX };
};

}

#endif
