#ifndef REND_DATA_ARRAY_H
#define REND_DATA_ARRAY_H

#include <assert.h>
#include <limits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <new>

#include "core/data_structures/data_array_base.h"

namespace rend
{

template<class T>
class DataArrayIterator
{
public:
    DataArrayIterator(DataArrayBase& container, uint32_t start)
        : _container(container),
          _current(start)
    {
        // Seek first valid element
        while (_current != _container.max_used())
        {
            DataArrayHandle handle = _container.handles()[_current];
            if (_container.check_valid(handle))
            {
                break;
            }

            ++_current;
        }
    }

    // Seek next valid element
    DataArrayIterator<T> operator++(void)
    {
        // Using max used instead of count or capacity because we operate a free list within a chunk of memory.
        // All we know is that _max_used have been allocated and MAY be valid. No need to go beyond that.
        while (_current != _container.max_used())
        {
            ++_current;
            DataArrayHandle handle = _container.handles()[_current];
            if (_container.check_valid(handle))
            {
                break;
            }
        }

        return *this;
    }

    bool operator!=(const DataArrayIterator& other)
    {
        return _current != other._current;
    }

    const T& operator*(void)
    {
        return static_cast<T*>(_container.data())[_current];
    }

private:
    DataArrayBase& _container;
    size_t         _current{ 0 };
};


template<class DataItemType>
class DataArray : public DataArrayBase
{
public:
    DataArray(size_t capacity)
        : DataArrayBase(sizeof(DataItemType), capacity)
    {
    }

    DataArray(void)
        : DataArray(c_default_capacity)
    {
    }

    ~DataArray(void)
    {
        clear();
    };

    // Constructs an object in place and returns a handle reference to it.
    template<typename... Args>
    DataArrayHandle allocate(Args&&... args)
    {
        DataArrayHandle handle = _allocate();

        new (&static_cast<DataItemType*>(_data)[_get_idx(handle)]) DataItemType(std::forward<Args>(args)...);

        return handle;
    }

    // Destructs the stored object and sets the passed handle to invalid.
    void deallocate(DataArrayHandle handle)
    {
        uint64_t idx = _get_idx(handle);
        uint64_t gen = _get_gen(handle);

        static_cast<DataItemType*>(_data)[idx].~DataItemType();

        _deallocate(idx, gen);
    }

    void clear(void)
    {
        for(uint32_t i{ 0 }; i < _count; ++i)
        {
            if (!check_valid(_handles[i]))
            {
                continue;
            }

            deallocate(_handles[i]);
        }
    }

    DataItemType* get(DataArrayHandle handle) const
    {
        if (!check_valid(handle))
        {
            return nullptr;
        }

        uint64_t idx = _get_idx(handle);
        return &static_cast<DataItemType*>(_data)[idx];
    }

    DataArrayIterator<DataItemType> begin(void) { return DataArrayIterator<DataItemType>(*this, 0); }
    DataArrayIterator<DataItemType> end(void)   { return DataArrayIterator<DataItemType>(*this, _count); }
};

template<class DataItemType>
class DataArrayExternal : public DataArrayBase
{
public:
    DataArrayExternal(size_t capacity)
        : DataArrayBase(sizeof(DataItemType), capacity)
    {
    }

    DataArrayExternal(void)
        : DataArrayExternal(c_default_capacity)
    {
    }

    ~DataArrayExternal(void)
    {
    }

    // Returns a handle and the item for external construction/destruction
    std::pair<DataArrayHandle, DataItemType*> allocate(void)
    {
        //uint32_t idx = _next_idx();
        //if (idx == invalid_handle)
        //{
        //    return { invalid_handle, nullptr };
        //}

        //uint32_t gen = _get_gen(_handles[idx]);

        //_handles[idx] = _make_handle(++gen, idx);

        //++_count;

        //return { _handles[idx], &static_cast<DataItemType*>(_data)[idx] };
        //
        DataArrayHandle handle = _allocate();
        return { handle, &static_cast<DataItemType*>(_data)[_get_idx(handle)] };
    }

    // Unallocated the item from the DataArray, and returns the item for external construction/destruction
    DataItemType& deallocate(DataArrayHandle handle)
    {
        uint64_t idx = _get_idx(handle);
        uint64_t gen = _get_gen(handle);

        DataItemType& value = static_cast<DataItemType*>(_data)[idx];

        _deallocate(idx, gen);

        //if( _free_head != invalid_handle )
        //{
        //    // There are some open nodes on the free list
        //    uint32_t free_head_idx = _get_idx(_free_head);
        //    uint32_t free_head_gen = _get_gen(_free_head);

        //    _handles[idx] = _make_invalid_handle(free_head_gen, free_head_idx);
        //    _free_head = _make_invalid_handle(gen, idx);
        //}
        //else
        //{
        //    // There are no open nodes on the free list
        //    _handles[idx] = _make_invalid_handle(gen, idx);
        //    _free_head = _handles[idx];
        //}

        //`--_count;
        return value;
    }

    void clear(void)
    {
        for(uint32_t i{ 0 }; i < _count; ++i)
        {
            if (!check_valid(_handles[i]))
            {
                continue;
            }

            deallocate(_handles[i]);
        }
    }

    DataItemType* get(DataArrayHandle handle) const
    {
        if (!check_valid(handle))
        {
            return nullptr;
        }

        uint64_t idx = _get_idx(handle);
        return &static_cast<DataItemType*>(_data)[idx];
    }

    DataArrayIterator<DataItemType> begin(void) { return DataArrayIterator<DataItemType>(*this, 0); }
    DataArrayIterator<DataItemType> end(void)   { return DataArrayIterator<DataItemType>(*this, _count); }
};


}

#endif
