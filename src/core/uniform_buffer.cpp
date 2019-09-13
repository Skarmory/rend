#include "uniform_buffer.h"

using namespace rend;

UniformBuffer::UniformBuffer(DeviceContext* context)
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
    if(!create_uniform_buffer_api(bytes))
    {
        // TODO: Log
        return false;
    }

    return true;
}
