#include "core/shader.h"

using namespace rend;

Shader::Shader(const std::string& name, size_t size_bytes, ShaderStage type)
    :
        GPUResource(name),
        _bytes(size_bytes),
        _type(type)
{
}

ShaderStage Shader::type(void) const
{
    return _type;
}
