#ifdef USE_VULKAN
#ifndef REND_VULKAN_INDEX_BUFFER_H
#define REND_VULKAN_INDEX_BUFFER_H

#include "index_buffer_base.h"
#include "vulkan_gpu_buffer.h"
#include "rend_defs.h"

#include <cstdint>
#include <cstddef>

namespace rend
{

class VulkanIndexBuffer : public VulkanGPUBuffer, public IndexBufferBase
{
public:
    VulkanIndexBuffer(void) = default;
    ~VulkanIndexBuffer(void) = default;

    VulkanIndexBuffer(const VulkanIndexBuffer&) = delete;
    VulkanIndexBuffer(VulkanIndexBuffer&&) = delete;
    VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;
    VulkanIndexBuffer& operator=(VulkanIndexBuffer&&) = delete;

protected:
    StatusCode create_index_buffer_api(uint32_t indices_count, size_t index_size);
};

}

#endif
#endif
