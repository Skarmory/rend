#ifndef SHADER_H
#define SHADER_H

#include <vulkan/vulkan.h>

namespace rend
{

class LogicalDevice;

enum class ShaderType
{
    VERTEX,
    FRAGMENT
};

class Shader
{
    friend class LogicalDevice;

public:
    Shader(const Shader&) = delete;
    Shader(Shader&&)      = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    VkShaderModule get_handle(void) const;

private:
    Shader(LogicalDevice* device, const void* code, uint32_t size_bytes, ShaderType type);
    ~Shader(void);

private:
    VkShaderModule _vk_module;

    LogicalDevice* _device;
    ShaderType _type;
};

}

#endif
