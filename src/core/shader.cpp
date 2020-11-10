#include "shader.h"

#include "device_context.h"

using namespace rend;

bool Shader::create_shader(const void* code, const uint32_t size_bytes, ShaderType type)
{
    auto& ctx = DeviceContext::instance();

    _handle = ctx.create_shader(type, code, size_bytes);

    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    _type = type;
    _bytes = size_bytes;

    return true;
}

ShaderHandle Shader::get_handle(void) const
{
    return _handle;
}

ShaderType Shader::get_type(void) const
{
    return _type;
}
