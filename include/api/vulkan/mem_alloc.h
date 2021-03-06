#ifndef REND_MEM_ALLOC_H
#define REND_MEM_ALLOC_H

#include <cstddef>
#include <cstdint>

namespace rend
{
class VulkanIndexBuffer;
class VulkanVertexBuffer;
}

namespace rend::vkal::memory
{

class MemAllocatorBase;

/*
 * Contains data abstracting a GPU memory allocation from a memory block.
 */
class MemAlloc
{
public:
    MemAlloc(MemAllocatorBase& owner, uint32_t offset, size_t bytes);
    ~MemAlloc(void);

    MemAlloc(const MemAlloc& other)            = default;
    MemAlloc(MemAlloc&& other)                 = default;
    MemAlloc& operator=(const MemAlloc& other) = default;
    MemAlloc& operator=(MemAlloc&& other)      = default;

    // Accessors
    size_t   size(void) const { return _size_bytes; }
    uint32_t offset(void) const { return _offset; }

private:
    uint32_t          _offset     { 0 };
    size_t            _size_bytes { 0 };
    MemAllocatorBase* _owner      { nullptr };
};

}

#endif
