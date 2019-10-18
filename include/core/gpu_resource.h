#ifndef REND_GPU_RESOURCE_H
#define REND_GPU_RESOURCE_H

#include <cstddef>

class GPUResource
{
public:
    GPUResource(void);
    ~GPUResource(void);

    size_t bytes(void) const;

protected:
    size_t _bytes;
};

#endif
