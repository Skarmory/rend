#include "shader.h"

using namespace rend;

Shader::Shader(DeviceContext& context)
    :
#ifdef USE_VULKAN
        VulkanShader(context)
#endif
{
}

Shader::~Shader(void)
{
}

bool Shader::create_shader(const void* code, uint32_t size_bytes, ShaderType type)
{
    if(create_shader_api(code, size_bytes, type) != StatusCode::SUCCESS)
    {
        return false;
    }

    _type = type;
    _bytes = size_bytes;

    return true;
}
