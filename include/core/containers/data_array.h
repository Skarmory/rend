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

#include "core/alloc/allocator.h"
#include "core/containers/data_array_base.h"

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

    bool operator!=(const DataArrayIterator& other) const
    {
        return _current != other._current;
    }

    T& operator*(void) const
    {
        return static_cast<T*>(_container.data())[_current];
    }

    DataArrayHandle handle(void) const
    {
        return _container.handles()[_current];
    }

private:
    DataArrayBase& _container;
    size_t         _current{ 0 };
};

template<class T>
class DataArrayConstIterator
{
public:
    DataArrayConstIterator(const DataArrayBase& container, uint32_t start)
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
    DataArrayConstIterator<T> operator++(void)
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

    bool operator!=(const DataArrayConstIterator& other) const
    {
        return _current != other._current;
    }

    const T& operator*(void) const
    {
        return static_cast<const T*>(_container.data())[_current];
    }

    DataArrayHandle handle(void) const
    {
        return _container.handles()[_current];
    }

private:
    const DataArrayBase& _container;
    size_t               _current{ 0 };
};


template<class DataItemType, class AllocatorType = Allocator<DataItemType>>
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
    [[nodiscard]]
    DataArrayHandle allocate(Args&&... args)
    {
        DataArrayHandle handle = _allocate();

        _allocator.construct(&static_cast<DataItemType*>(_data)[_get_idx(handle)], std::forward<Args>(args)...);

        return handle;
    }

    // Destructs the stored object and sets the passed handle to invalid.
    void deallocate(DataArrayHandle handle)
    {
        uint64_t idx = _get_idx(handle);
        uint64_t gen = _get_gen(handle);

        _allocator.destruct(&static_cast<DataItemType*>(_data)[idx]);

        _deallocate(idx, gen);
    }

    void clear(void)
    {
        for(uint32_t i{ 0 }; i < _max_used; ++i)
        {
            if (!check_valid(_handles[i]))
            {
                continue;
            }

            deallocate(_handles[i]);
        }
    }

    template<typename Func>
    DataArrayHandle find(Func f) const
    {
        for(auto it = cbegin(); it != cend(); ++it)
        {
            if(f(*it))
            {
                return it.handle();
            }
        }

        return invalid_handle;
    }

    DataItemType* get(DataArrayHandle handle) const
    {
        if(!check_valid(handle))
        {
            return nullptr;
        }

        return &static_cast<DataItemType*>(_data)[_get_idx(handle)];
    }

    DataArrayIterator<DataItemType> begin(void) { return DataArrayIterator<DataItemType>(*this, 0); }
    DataArrayIterator<DataItemType> end(void)   { return DataArrayIterator<DataItemType>(*this, _count); }

    DataArrayConstIterator<DataItemType> cbegin(void) const { return DataArrayConstIterator<DataItemType>(*this, 0); }
    DataArrayConstIterator<DataItemType> cend(void)   const { return DataArrayConstIterator<DataItemType>(*this, _count); }

private:
    AllocatorType _allocator;
};

}

#endif
