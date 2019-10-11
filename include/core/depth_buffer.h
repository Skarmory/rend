#ifndef REND_DEPTH_BUFFER_H
#define REND_DEPTH_BUFFER_H

#ifdef USE_VULKAN
#include "vulkan_depth_buffer.h"
#endif

namespace rend
{

class DeviceContext;

#ifdef USE_VULKAN
class DepthBuffer : public VulkanDepthBuffer
#endif
{
public:
    explicit DepthBuffer(DeviceContext& context);
    ~DepthBuffer(void);

    DepthBuffer(const DeviceContext&)            = delete;
    DepthBuffer(DeviceContext&&)                 = delete;
    DepthBuffer& operator=(const DeviceContext&) = delete;
    DepthBuffer& operator=(DeviceContext&&)      = delete;

    bool create_depth_buffer(uint32_t width, uint32_t height);
    void destroy_depth_buffer(void);
};

}

#endif
