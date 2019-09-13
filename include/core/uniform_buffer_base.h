#ifndef REND_UNIFORM_BUFFER_BASE_H
#define REND_UNIFORM_BUFFER_BASE_H

#include <cstdint>
#include <cstddef>

namespace rend
{

class UniformBufferBase
{
public:
    UniformBufferBase(void);
    virtual ~UniformBufferBase(void);

    UniformBufferBase(const UniformBufferBase&) = delete;
    UniformBufferBase(UniformBufferBase&&) = delete;
    const UniformBufferBase& operator=(const UniformBufferBase&) = delete;
    const UniformBufferBase& operator=(UniformBufferBase&&) = delete;

    size_t bytes(void) const;

protected:
    size_t _bytes;
};

}

#endif
