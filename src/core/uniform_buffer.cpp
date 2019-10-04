#include "uniform_buffer.h"

#include "rend_defs.h"

using namespace rend;

UniformBuffer::UniformBuffer(DeviceContext& context)
    :
#ifdef USE_VULKAN
        VulkanUniformBuffer(context)
#endif
{
}

UniformBuffer::~UniformBuffer(void)
{
}

bool UniformBuffer::create_uniform_buffer(size_t bytes)
{
    if(create_uniform_buffer_api(bytes) != StatusCode::SUCCESS)
    {
        // TODO: Log
        return false;
    }

    return true;
}
