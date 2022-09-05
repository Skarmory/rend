#include "core/containers/data_array_base.h"

#include <cstdlib>
#include <cstring>

using namespace rend;

DataArrayBase::DataArrayBase(size_t item_size, size_t capacity)
    :   _data(malloc(item_size * capacity)),
        _handles(static_cast<DataArrayHandle*>(malloc(sizeof(DataArrayHandle) * capacity))),
        _capacity(capacity)
{
    for(int i = 0; i < capacity; ++i)
    {
        _handles[i] = invalid_handle;
    }
}

DataArrayBase::DataArrayBase(DataArrayBase&& other)
{
    if (this != &other)
    {
        _data      = other._data;
        _handles   = other._handles;
        _capacity  = other._capacity;
        _max_used  = other._max_used;
        _count     = other._count;
        _free_head = other._free_head;

        other._data      = nullptr;
        other._handles   = nullptr;
        other._capacity  = 0;
        other._max_used  = 0;
        other._count     = 0;
        other._free_head = invalid_handle;
    }
}

DataArrayBase& DataArrayBase::operator=(DataArrayBase&& other) noexcept
{
    if (this != &other)
    {
        _data      = other._data;
        _handles   = other._handles;
        _capacity  = other._capacity;
        _max_used  = other._max_used;
        _count     = other._count;
        _free_head = other._free_head;

        other._data      = nullptr;
        other._handles   = nullptr;
        other._capacity  = 0;
        other._max_used  = 0;
        other._count     = 0;
        other._free_head = invalid_handle;
    }

    return *this;
}

DataArrayBase::~DataArrayBase(void)
{
    free(_handles);
    free(_data);
}

// Check that the key for the item matches the data
bool DataArrayBase::check_valid(DataArrayHandle handle) const
{
    uint64_t key = _get_gen(handle);
    uint64_t idx = _get_idx(handle);

    if(is_invalid_handle(handle))
    {
        return false;
    }

    if (key == c_generation_mask || idx > _capacity)
    {
        return false;
    }

    const DataArrayHandle check_handle = _handles[idx];

    return _get_gen(handle) == _get_gen(check_handle);
}

// Get the raw array of data.
void* DataArrayBase::data(void) const
{
    return _data;
}

// Get the raw array of handles.
DataArrayHandle* DataArrayBase::handles(void) const
{
    return _handles;
}

uint32_t DataArrayBase::size(void) const
{
    return _count;
}

uint32_t DataArrayBase::max_used(void) const
{
    return _max_used;
} 

uint64_t DataArrayBase::_get_gen(DataArrayHandle handle) const
{
    return (handle & c_generation_mask) >> c_generation_shift;
}

uint64_t DataArrayBase::_get_idx(DataArrayHandle handle) const
{
    return handle & c_index_mask;
}

// Construct a handle by encoding info into it
DataArrayHandle DataArrayBase::_make_handle(uint64_t generation, uint64_t idx) const
{
   DataArrayHandle new_handle{ (generation << c_generation_shift) |  idx };
   return new_handle;
}

// Construct a handle that's invalid, but points towards a free index
DataArrayHandle DataArrayBase::_make_invalid_handle(uint64_t generation, uint64_t idx) const
{
    return c_invalid_mask | _make_handle(generation, idx);
}

DataArrayHandle DataArrayBase::_allocate(void)
{
    DataArrayHandle handle = invalid_handle;

    if(_free_head == invalid_handle) // No free list to pick from
    {
        handle = _make_handle(0, _max_used);
        _handles[_max_used] = handle;
        ++_max_used;
    }
    else // Grab from free list
    {
        uint64_t free_head_idx = _get_idx(_free_head);
        uint64_t free_head_gen = _get_gen(_free_head);

        ++free_head_gen;

        handle = _make_handle(free_head_gen, free_head_idx);

        _free_head = _handles[free_head_idx];
        _handles[free_head_idx] = handle;
    }

    ++_count;

    return handle;
}

void DataArrayBase::_deallocate(uint64_t idx, uint64_t gen)
{
    if(_free_head != invalid_handle)
    {
        // Free head not empty
        _handles[idx] = _free_head;
        _free_head = _make_invalid_handle(gen, idx);
    }
    else // No free list
    {
        _free_head = _make_invalid_handle(gen, idx);
        _handles[idx] = invalid_handle;
    }

    --_count;
}
