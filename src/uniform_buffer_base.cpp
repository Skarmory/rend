#include "uniform_buffer_base.h"

using namespace rend;

UniformBufferBase::UniformBufferBase(void)
    : _bytes(0)
{
}

UniformBufferBase::~UniformBufferBase(void)
{
}

size_t UniformBufferBase::bytes(void) const
{
    return _bytes;
}
