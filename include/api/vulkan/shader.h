#ifndef REND_SHADER_H
#define REND_SHADER_H

#include <vulkan.h>

namespace rend
{

class DeviceContext;

enum class ShaderType
{
    VERTEX,
    FRAGMENT,
    UNKNOWN
};

class Shader
{
public:
    Shader(DeviceContext* context);
    ~Shader(void);
    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    bool create_shader(const void* code, uint32_t size_bytes, ShaderType type);

    VkShaderModule get_handle(void) const;
    ShaderType     get_shader_type(void) const;

private:
    DeviceContext* _context;
    ShaderType     _type;

    VkShaderModule _vk_module;
};

}

#endif
