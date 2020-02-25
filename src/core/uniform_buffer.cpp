#include "uniform_buffer.h"

#include "rend_defs.h"

using namespace rend;

bool UniformBuffer::create_uniform_buffer(size_t bytes)
{
    if(create_uniform_buffer_api(bytes) != StatusCode::SUCCESS)
    {
        // TODO: Log
        return false;
    }

    return true;
}
