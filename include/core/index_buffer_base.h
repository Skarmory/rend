#ifndef REND_INDEX_BUFFER_BASE_H
#define REND_INDEX_BUFFER_BASE_H

#include <cstdint>
#include <cstddef>

namespace rend
{

class IndexBufferBase
{
public:
    IndexBufferBase(void);
    virtual ~IndexBufferBase(void);

    IndexBufferBase(const IndexBufferBase&) = delete;
    IndexBufferBase(IndexBufferBase&&) = delete;
    IndexBufferBase& operator=(const IndexBufferBase&) = delete;
    IndexBufferBase& operator=(IndexBufferBase&&) = delete;

    uint32_t count(void) const;

protected:
    uint32_t _count;
};

}

#endif
