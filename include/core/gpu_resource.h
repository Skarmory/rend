#ifndef REND_GPU_RESOURCE_H
#define REND_GPU_RESOURCE_H

#include <cstddef>
#include <string>

class GPUResource
{
public:
    GPUResource(void);
    ~GPUResource(void);

    size_t             bytes(void) const;

#ifdef DEBUG
    const std::string& dbg_name(void) const;
    void               dbg_name(const std::string& name);
#endif

protected:
    size_t      _bytes{ 0 };

#ifdef DEBUG
    std::string _dbg_name{ "unnamed" };
#endif
};

#endif
