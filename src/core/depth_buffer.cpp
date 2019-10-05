#include "depth_buffer.h"

using namespace rend;

DepthBuffer::DepthBuffer(DeviceContext& context)
    :
#ifdef USE_VULKAN
        VulkanDepthBuffer(context)
#endif
{
}

DepthBuffer::~DepthBuffer(void)
{
}

bool DepthBuffer::create_depth_buffer(uint32_t width, uint32_t height)
{
    if(create_depth_buffer_api(width, height) != StatusCode::SUCCESS)
    {
        return false;
    }

    return true;

}
