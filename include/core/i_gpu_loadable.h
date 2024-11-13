#ifndef REND_CORE_I_GPU_LOADABLE_H
#define REND_CORE_I_GPU_LOADABLE_H

#include <vector>

namespace rend
{

class IGPULoadable
{
    public:
        /**
         * Store data CPU-side on this resource.
         */
        virtual void store_data(char* data, size_t size_bytes) = 0;

        /**
         * Load stored data to GPU.
         */
        virtual void load_to_gpu(void) = 0;
};

}

#endif
