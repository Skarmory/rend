#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>

namespace rend
{

class DeviceContext;
class GPUBuffer;

enum class BufferType
{
    VERTEX,
    INDEX,
    STAGING
};

class Buffer
{
public:
    Buffer(DeviceContext* context, size_t size_bytes, BufferType buffer_type);
    ~Buffer(void);

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&)      = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&)      = delete;

    GPUBuffer* get_gpu_buffer(void) const;

private:
    DeviceContext* _context;
    GPUBuffer*     _gpu_buffer;
    BufferType     _type;
};

}

#endif
