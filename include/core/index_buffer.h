#ifndef REND_INDEX_BUFFER_H
#define REND_INDEX_BUFFER_H

#include <cstdint>
#include <cstddef>

#include "gpu_buffer_base.h"
#include "rend_defs.h"

namespace rend
{

class IndexBuffer : public GPUBufferBase
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
