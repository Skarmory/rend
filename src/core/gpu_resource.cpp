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

#ifdef DEBUG
const std::string& GPUResource::dbg_name(void) const
{
    return _dbg_name;
}

void GPUResource::dbg_name(const std::string& name)
{
    _dbg_name = name;
}
#endif
