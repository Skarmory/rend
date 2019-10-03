#ifdef USE_VULKAN
#ifndef REND_VULKAN_VERTEX_BUFFER_H
#define REND_VULKAN_VERTEX_BUFFER_H

#include "vertex_buffer_base.h"

#include <cstdint>
#include <cstddef>

namespace rend
{

class DeviceContext;
class VulkanGPUBuffer;

class VulkanVertexBuffer : public VertexBufferBase
{
public:
    VulkanVertexBuffer(DeviceContext* context);
    virtual ~VulkanVertexBuffer(void);

    VulkanVertexBuffer(const VulkanVertexBuffer&) = delete;
    VulkanVertexBuffer(VulkanVertexBuffer&&) = delete;
    const VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;
    const VulkanVertexBuffer& operator=(VulkanVertexBuffer&&) = delete;

    VulkanGPUBuffer* gpu_buffer(void) const;

protected:
    bool create_vertex_buffer_api(uint32_t vertices_count, size_t vertex_size);

private:
    DeviceContext*   _context;
    VulkanGPUBuffer* _buffer;
};

}

#endif
#endif

