#ifndef REND_GPU_RESOURCE_H
#define REND_GPU_RESOURCE_H

#include <cstddef>
#include <string>

namespace rend
{

class GPUResource
{
public:
    explicit GPUResource(const std::string& name);
    GPUResource(void);
    virtual ~GPUResource(void) = default;

    const std::string& name(void) const;
    void               name(const std::string& name);

    size_t id(void) const;

private:
    std::string _name;
    std::size_t _id;
};

}

#endif
