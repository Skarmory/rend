#ifdef USE_VULKAN
#ifndef REND_VULKAN_INDEX_BUFFER_H
#define REND_VULKAN_INDEX_BUFFER_H

#include "index_buffer_base.h"

namespace rend
{

class DeviceContext;
class GPUBuffer;

class VulkanIndexBuffer : public IndexBufferBase
{
public:
    VulkanIndexBuffer(DeviceContext* context);
    virtual ~VulkanIndexBuffer(void);

    VulkanIndexBuffer(const VulkanIndexBuffer&) = delete;
    VulkanIndexBuffer(VulkanIndexBuffer&&) = delete;
    VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;
    VulkanIndexBuffer& operator=(VulkanIndexBuffer&&) = delete;

    GPUBuffer* gpu_buffer(void) const;

protected:
    bool create_index_buffer_api(/*void* data,*/ uint32_t indices_count, size_t index_size);

private:
    DeviceContext* _context;
    GPUBuffer*     _buffer;
};

}

#endif
#endif
