#include "mem_block.h"

#include "device_context.h"
#include "logical_device.h"
#include "mem_alloc.h"
#include "renderer.h"
#include "vulkan_helper_funcs.h"

#include <cassert>
#include <cstring>
#include <vector>

using namespace rend;
using namespace rend::vkal;
using namespace rend::vkal::memory;

/* ----- __MemBlock ----- */

struct MemBlock::__MemBlock
{
    __MemBlock(void) = default;

    __MemBlock(const __MemBlock&)          = delete;
    __MemBlock& operator=(const MemBlock&) = delete;

    __MemBlock(__MemBlock&& other);
    __MemBlock& operator=(__MemBlock&& other);


    // Accessors
    bool      compatible(const VkMemoryRequirements& memory_reqs, VkMemoryPropertyFlags memory_props, const VkMemoryType& memory_type_index, ResourceUsage resource_usage) const;
    size_t    capacity(void) const;

    // Mutators
    StatusCode create(Renderer& renderer, size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage);
    MemAlloc*  create_mem_alloc(uint32_t offset, size_t size_bytes);
    bool       write(void* data, size_t size_bytes, uint32_t offset);
    bool       write(void* data, size_t size_bytes, uint32_t offset, void* resource);

    // Member variables
    Renderer*             _renderer; // TODO: Singletonise this?
    size_t                _capacity_bytes      { 0 };
    size_t                _used_bytes          { 0 };
    ResourceUsage         _resource_usage      { NO_RESOURCE };
    VkMemoryType          _memory_type         { };
    VkMemoryRequirements  _memory_requirements { };
    VkMemoryPropertyFlags _memory_properties   { 0 };
    VkDeviceMemory        _vk_memory           { VK_NULL_HANDLE };
    std::vector<MemAlloc> _allocs;
};

MemBlock::__MemBlock::__MemBlock(MemBlock::__MemBlock&& other)
{
    if(this != &other)
    {
        this->_renderer            = other._renderer;
        this->_capacity_bytes      = other._capacity_bytes;
        this->_used_bytes          = other._used_bytes;
        this->_resource_usage      = other._resource_usage;
        this->_memory_type         = other._memory_type;
        this->_memory_requirements = other._memory_requirements;
        this->_memory_properties   = other._memory_properties;
        this->_vk_memory           = other._vk_memory;
        this->_allocs              = std::move(other._allocs);

        other._renderer            = nullptr;
        other._capacity_bytes      = 0;
        other._used_bytes          = 0;
        other._resource_usage      = NO_RESOURCE;
        other._memory_type         = {};
        other._memory_requirements = {};
        other._memory_properties   = 0;
        other._vk_memory           = VK_NULL_HANDLE;
    }
}

MemBlock::__MemBlock& MemBlock::__MemBlock::operator=(MemBlock::__MemBlock&& other)
{
    if(this != &other)
    {
        this->_renderer            = other._renderer;
        this->_capacity_bytes      = other._capacity_bytes;
        this->_used_bytes          = other._used_bytes;
        this->_resource_usage      = other._resource_usage;
        this->_memory_type         = other._memory_type;
        this->_memory_requirements = other._memory_requirements;
        this->_memory_properties   = other._memory_properties;
        this->_vk_memory           = other._vk_memory;
        this->_allocs              = std::move(other._allocs);

        other._renderer            = nullptr;
        other._capacity_bytes      = 0;
        other._used_bytes          = 0;
        other._resource_usage      = NO_RESOURCE;
        other._memory_type         = {};
        other._memory_requirements = {};
        other._memory_properties   = 0;
        other._vk_memory           = VK_NULL_HANDLE;
    }

    return *this;
} 

StatusCode MemBlock::__MemBlock::create(Renderer& renderer, size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage)
{
    VkMemoryAllocateInfo info = vulkan_helpers::gen_memory_allocate_info();
    info.allocationSize       = block_size;
    info.memoryTypeIndex      = _memory_type.heapIndex;

    _vk_memory = DeviceContext::instance().get_device()->allocate_memory(info);
    if(_vk_memory == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    _renderer = &renderer;
    _capacity_bytes = block_size;
    _resource_usage = resource_usage;
    _memory_type = memory_type;
    _memory_requirements = memory_requirements;
    _memory_properties = memory_properties;

    return StatusCode::SUCCESS;
}

bool MemBlock::__MemBlock::compatible(const VkMemoryRequirements& memory_requirements, VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage) const
{
    return ((resource_usage == _resource_usage) &&
            (memory_type.heapIndex == _memory_type.heapIndex) &&
            ((memory_properties & _memory_properties) == memory_properties) &&
            (memory_requirements.alignment == _memory_requirements.alignment));
}

size_t MemBlock::__MemBlock::capacity(void) const
{
    return _capacity_bytes;
}

MemAlloc* MemBlock::__MemBlock::create_mem_alloc(uint32_t offset, size_t size_bytes)
{
    _allocs.emplace_back(offset, size_bytes);
    return &_allocs.back();
}

bool MemBlock::__MemBlock::write(void* data, size_t size_bytes, uint32_t offset)
{
    // TODO: Check size can fit
    void* mapped { nullptr };
    DeviceContext::instance().get_device()->map_memory(_vk_memory, size_bytes, offset, &mapped);
    memcpy(mapped, data, size_bytes);
    DeviceContext::instance().get_device()->unmap_memory(_vk_memory);

    _used_bytes += size_bytes;

    return true;
}

bool MemBlock::__MemBlock::write(void* data, size_t size_bytes, uint32_t offset, void* resource)
{
    _renderer->load(resource, _resource_usage, data, size_bytes, offset);
    _used_bytes += size_bytes;
    return true;
}

/* ----- MemBlock ----- */

MemBlock::MemBlock(void)
{
}

MemBlock::~MemBlock(void)
{
    // Default constructor won't work with unique_ptr
}

MemBlock::MemBlock(MemBlock&& other)
{
    if(this != &other)
    {
        this->_ = std::move(other._);
    }
}

MemBlock& MemBlock::operator=(MemBlock&& other)
{
    if(this != &other)
    {
        this->_ = std::move(other._);
    }

    return *this;
}

// TODO: Get rid of renderer dep injection
StatusCode MemBlock::create(Renderer& renderer, size_t block_size, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage)
{
    _ = std::make_unique<__MemBlock>();
    _->create(renderer, block_size, memory_requirements, memory_properties, memory_type, resource_usage);

    return StatusCode::SUCCESS;
}

bool MemBlock::compatible(const VkMemoryRequirements& memory_requirements, VkMemoryPropertyFlags memory_properties, const VkMemoryType& memory_type, ResourceUsage resource_usage)
{
    return _->compatible(memory_requirements, memory_properties, memory_type, resource_usage);
}

size_t MemBlock::capacity(void) const
{
    return _->_capacity_bytes;
}

MemAlloc* MemBlock::create_mem_alloc(uint32_t offset, size_t size_bytes)
{
    return _->create_mem_alloc(offset, size_bytes);
}

bool MemBlock::write(void* data, size_t size_bytes, uint32_t offset)
{
    return _->write(data, size_bytes, offset);
}

bool MemBlock::write(void* data, size_t size_bytes, uint32_t offset, void* resource)
{
    return _->write(data, size_bytes, offset, resource);
}
