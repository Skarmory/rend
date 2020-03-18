#include "mem_heap.h"

#include "mem_block.h"

#include <cassert>

using namespace rend;
using namespace rend::vkal;
using namespace rend::vkal::memory;

/* ----- __MemHeap ----- */

struct MemHeap::__MemHeap
{
    __MemHeap(void) = default;
    __MemHeap(size_t available_bytes);

    __MemHeap(const __MemHeap&)            = delete;
    __MemHeap& operator=(const __MemHeap&) = delete;

    __MemHeap(__MemHeap&& other);
    __MemHeap& operator=(__MemHeap&& other);

    MemBlock* create_block(Renderer& renderer, size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage);
    MemBlock* find_block(const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage);

    size_t                _available_bytes { 0 };
    size_t                _used_bytes      { 0 };
    std::vector<MemBlock> _mem_blocks;
};

MemHeap::__MemHeap::__MemHeap(size_t available_bytes)
    : _available_bytes(available_bytes)
{
}

MemHeap::__MemHeap::__MemHeap(MemHeap::__MemHeap&& other)
{
    if(this != &other)
    {
        this->_available_bytes = other._available_bytes;
        this->_used_bytes      = other._used_bytes;
        this->_mem_blocks       = std::move(other._mem_blocks);
    }
}

MemHeap::__MemHeap& MemHeap::__MemHeap::operator=(MemHeap::__MemHeap&& other)
{
    if(this != &other)
    {
        this->_available_bytes = other._available_bytes;
        this->_used_bytes      = other._used_bytes;
        this->_mem_blocks       = std::move(other._mem_blocks);
    }

    return *this;
}

// TODO: Get rid of renderer dep injection
MemBlock* MemHeap::__MemHeap::create_block(Renderer& renderer, size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage)
{
    MemBlock& block = _mem_blocks.emplace_back();

    if(block.create(renderer, block_size, memory_requirements, memory_properties, memory_type, resource_usage) != StatusCode::SUCCESS)
    {
        return &block;
    }

    assert(true && "Memory error: Failed to create MemBlock");

    return nullptr;
}

MemBlock* MemHeap::__MemHeap::find_block(const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage)
{
    for(MemBlock& block : _mem_blocks)
    {
        if(block.compatible(memory_requirements, memory_properties, memory_type, resource_usage))
        {
            return &block;
        }
    }

    return nullptr;
}

/* ----- MemHeap -----*/

MemHeap::MemHeap(size_t available_bytes)
{
    _ = std::make_unique<__MemHeap>(available_bytes);
}

MemHeap::~MemHeap(void)
{
    // Default destructor won't work with unique_ptr
}

MemHeap::MemHeap(MemHeap&& other)
{
    if(this != &other)
    {
        this->_ = std::move(other._);
    }
}

MemHeap& MemHeap::operator=(MemHeap&& other)
{
    if(this != &other)
    {
        this-> _ = std::move(other._);
    }

    return *this;
}

MemBlock* MemHeap::create_block(Renderer& renderer, size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage)
{
    return _->create_block(renderer, block_size, memory_requirements, memory_properties, memory_type, resource_usage);
}

MemBlock* MemHeap::find_block(const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage)
{
    return _->find_block(memory_requirements, memory_properties, memory_type, resource_usage);
}
