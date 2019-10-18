#ifdef USE_VULKAN
#ifndef REND_VULKAN_VERTEX_BUFFER_H
#define REND_VULKAN_VERTEX_BUFFER_H

#include "vertex_buffer_base.h"
#include "vulkan_gpu_buffer.h"
#include "rend_defs.h"

#include <cstdint>
#include <cstddef>

namespace rend
{

class DeviceContext;

class VulkanVertexBuffer : public VulkanGPUBuffer, public VertexBufferBase
{
public:
    explicit VulkanVertexBuffer(DeviceContext& context);
    ~VulkanVertexBuffer(void);

    VulkanVertexBuffer(const VulkanVertexBuffer&) = delete;
    VulkanVertexBuffer(VulkanVertexBuffer&&) = delete;
    const VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;
    const VulkanVertexBuffer& operator=(VulkanVertexBuffer&&) = delete;

protected:
    StatusCode create_vertex_buffer_api(uint32_t vertices_count, size_t vertex_size);
};

}

#endif
#endif

