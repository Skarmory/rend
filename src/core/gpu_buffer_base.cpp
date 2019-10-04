#include "gpu_buffer_base.h"

using namespace rend;

GPUBufferBase::GPUBufferBase(void)
    : _bytes(0)
{
}

GPUBufferBase::~GPUBufferBase(void)
{
}

size_t GPUBufferBase::bytes(void) const
{
    return _bytes;
}

