#include "core/shader.h"

#include "core/device_context.h"
#include "core/rend_service.h"

#include <cassert>

using namespace rend;

Shader::Shader(const void* code, uint32_t size_bytes, ShaderStage type)
    : _bytes(size_bytes),
      _type(type)
{
    auto& ctx = *RendService::device_context();
    _handle = ctx.create_shader(type, code, size_bytes);
}

Shader::~Shader(void)
{
    auto& ctx = *RendService::device_context();
    ctx.destroy_shader(_handle);
}

ShaderHandle Shader::handle(void) const
{
    return _handle;
}

ShaderStage Shader::type(void) const
{
    return _type;
}
