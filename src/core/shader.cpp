#include "core/shader.h"

#include "core/device_context.h"

#include <cassert>

using namespace rend;

bool Shader::create(const void* code, const uint32_t size_bytes, ShaderStage type)
{
    assert(_handle == NULL_HANDLE && "Attempt to create a Shader that has already been created.");

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

void Shader::destroy(void)
{
    auto& ctx = DeviceContext::instance();
    ctx.destroy_shader(_handle);
    _handle = NULL_HANDLE;
}

ShaderHandle Shader::get_handle(void) const
{
    return _handle;
}

ShaderStage Shader::get_type(void) const
{
    return _type;
}
