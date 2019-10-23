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
    create_texture_base(width, height, 1, TextureFormat::D24_S8);

    if(create_depth_buffer_api(width, height) != StatusCode::SUCCESS)
    {
        return false;
    }

    return true;

}

void DepthBuffer::destroy_depth_buffer(void)
{
    destroy_texture_api();
}
