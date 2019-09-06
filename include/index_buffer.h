#ifndef REND_INDEX_BUFFER_H
#define REND_INDEX_BUFFER_H

#ifdef USE_VULKAN
#include "vulkan_index_buffer.h"
#endif

namespace rend
{

class DeviceContext;

#ifdef USE_VULKAN
class IndexBuffer : public VulkanIndexBuffer
#else
class IndexBuffer
#endif
{
public:
    IndexBuffer(DeviceContext* context);
    ~IndexBuffer(void);

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) = delete;

    bool create_index_buffer(/*void* data,*/ uint32_t indices_count, size_t index_size);
};

}

#endif
