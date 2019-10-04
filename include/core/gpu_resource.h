#ifndef REND_GPU_RESOURCE_H
#define REND_GPU_RESOURCE_H

#include <cstddef>

class GPUResource
{
public:
    GPUResource(void);
    virtual ~GPUResource(void);

    size_t bytes(void) const;

private:
    size_t _bytes;
};

#endif
