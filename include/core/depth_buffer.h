#ifndef REND_DEPTH_BUFFER_H
#define REND_DEPTH_BUFFER_H

#ifdef USE_VULKAN
#include "vulkan_depth_buffer.h"
#endif

namespace rend
{

#ifdef USE_VULKAN
class DepthBuffer : public VulkanDepthBuffer
#endif
{
public:
    DepthBuffer(void) = default;
    ~DepthBuffer(void) = default;

    DepthBuffer(const DepthBuffer&)            = delete;
    DepthBuffer(DepthBuffer&&)                 = delete;
    DepthBuffer& operator=(const DepthBuffer&) = delete;
    DepthBuffer& operator=(DepthBuffer&&)      = delete;

    bool create_depth_buffer(uint32_t width, uint32_t height);
    void destroy_depth_buffer(void);
};

}

#endif
