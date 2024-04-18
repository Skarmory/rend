#include "core/shader.h"

using namespace rend;

Shader::Shader(const std::string& name, size_t size_bytes, ShaderStage type, RendHandle rend_handle)
    :
        GPUResource(name),
        RendObject(rend_handle),
        _bytes(size_bytes),
        _type(type)
{
}

ShaderStage Shader::type(void) const
{
    return _type;
}
