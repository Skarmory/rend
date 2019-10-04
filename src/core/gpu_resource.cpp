#include "gpu_resource.h"

GPUResource::GPUResource(void)
    : _bytes(0)
{
}

GPUResource::~GPUResource(void)
{
}

size_t GPUResource::bytes(void) const
{
    return _bytes;
}
