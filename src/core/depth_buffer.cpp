#include "depth_buffer.h"

#include "device_context.h"
#include "vulkan_device_context.h"

using namespace rend;

bool DepthBuffer::create_depth_buffer(uint32_t width, uint32_t height)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());

    _handle = ctx.create_texture_2d(width, height, 1, 1, Format::D24_S8, ImageUsage::DEPTH_STENCIL);

    if(_handle == NULL_HANDLE)
    {
        return false;
    }

    _width = width;
    _height = height;
    _format = Format::D24_S8;

    return true;
}
