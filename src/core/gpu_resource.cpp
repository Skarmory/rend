#include "core/gpu_resource.h"

#include <functional>

using namespace rend;

namespace
{
    static uint32_t _unnamed_resource_count = 0;
}

GPUResource::GPUResource(const std::string& name)
    :
        _name(name),
        _id(std::hash<std::string>{}(name))
{
}

GPUResource::GPUResource(void)
    :
        GPUResource("Unnamed buffer " + std::to_string(::_unnamed_resource_count++))
{
}

const std::string& GPUResource::name(void) const
{
    return _name;
}

void GPUResource::name(const std::string& name)
{
    _name = name;
    _id = std::hash<std::string>{}(name);
}

size_t GPUResource::id(void) const
{
    return _id;
}
