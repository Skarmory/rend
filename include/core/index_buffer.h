#ifndef REND_INDEX_BUFFER_H
#define REND_INDEX_BUFFER_H

#ifdef USE_VULKAN
#include "vulkan_index_buffer.h"
#endif

#include <cstdint>
#include <cstddef>

namespace rend
{

#ifdef USE_VULKAN
class IndexBuffer : public VulkanIndexBuffer
#else
class IndexBuffer
#endif
{
public:
    IndexBuffer(void) = default;
    ~IndexBuffer(void) = default;

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) = delete;

    bool create_index_buffer(uint32_t indices_count, size_t index_size);
};

}

#endif
