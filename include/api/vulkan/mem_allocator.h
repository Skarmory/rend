#ifndef REND_MEM_ALLOCATOR_H
#define REND_MEM_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rend::vkal::memory
{

class MemAlloc;
class MemBlock;

/*
 * Proxy for interfacing with a memory allocation.
 * Holds information for tracking validity.
 */
class MemHandle
{
    friend class PoolAllocator;

public:
    MemHandle(MemAlloc& alloc, uint32_t index);
    ~MemHandle(void) = default;

    MemAlloc* operator->(void);

    uint32_t generation(void) const;
    uint32_t index(void) const;

private:
    uint32_t  _generation { 0 };
    uint32_t  _index      { 0 };
    MemAlloc* _allocation { nullptr };
};

/*
 * Base class for memory allocators.
 */
class MemAllocatorBase
{
public:
    MemAllocatorBase(MemBlock& block);
    virtual ~MemAllocatorBase(void) = default;

    virtual MemHandle allocate(size_t size_bytes) = 0;
    virtual void      deallocate(MemHandle handle) = 0;
    MemBlock*         block(void) const;

protected:
    std::vector<MemHandle> _handles;
    MemBlock*              _block { nullptr };
};

/*
 * A memory allocator that divides up a MemBlock into fixed size MemAllocs.
 */
class PoolAllocator : public MemAllocatorBase
{
public:
    PoolAllocator(MemBlock& block, size_t alloc_size);
    ~PoolAllocator(void) = default;

    [[nodiscard]] MemHandle allocate(size_t bytes) override;
    void                    deallocate(MemHandle handle) override;

private:
    size_t                _alloc_size { 0 };
    std::vector<uint32_t> _free_list;
};

}

#endif
