#include "shader_base.h"

using namespace rend;

ShaderBase::ShaderBase(void)
    : _type(ShaderType::VERTEX)
{
}

ShaderBase::~ShaderBase(void)
{
}

ShaderType ShaderBase::shader_type(void) const
{
    return _type;
}

