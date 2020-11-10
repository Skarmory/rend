#ifndef REND_SHADER_H
#define REND_SHADER_H

#include "rend_defs.h"
#include "vulkan_shader.h"

#include <cstdint>

namespace rend
{

class Shader : public VulkanShader
{
public:
    Shader(void) = default;
    ~Shader(void) = default;

    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    bool create_shader(const void* code, uint32_t size_bytes, ShaderType type);
};

}

#endif
