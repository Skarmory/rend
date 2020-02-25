#ifndef REND_SHADER_H
#define REND_SHADER_H

#ifdef USE_VULKAN
#include "vulkan_shader.h"
#endif

namespace rend
{

#ifdef USE_VULKAN
class Shader : public VulkanShader
#endif
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
