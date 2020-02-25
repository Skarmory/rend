#include "shader.h"

using namespace rend;

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
