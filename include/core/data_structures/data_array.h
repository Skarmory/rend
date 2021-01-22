#ifndef REND_DATA_ARRAY_H
#define REND_DATA_ARRAY_H

#include <assert.h>
#include <limits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>

namespace rend
{

typedef uint64_t DataArrayHandle;
static const DataArrayHandle idx_mask{ 0xffffffff };
static const DataArrayHandle key_mask{ 0x0000ffff00000000 };
static const DataArrayHandle unique_key_mask{ 0xffff000000000000 };

static const DataArrayHandle invalid_handle { unique_key_mask | key_mask | idx_mask };

constexpr bool is_invalid_handle(DataArrayHandle handle) { return (handle & key_mask) == key_mask; }

template<class DataItemType>
class DataArray;

//template<class DataItemType>
//class DataAccessor
//{
//public:
//    DataAccessor(void) {}
//    DataAccessor(DataArrayHandle handle, DataArray<DataItemType>& array)
//        : _handle(handle)
//        , _array(&array)
//    {
//    }
//
//    DataItemType* operator->(void)
//    {
//        return _array->get(_handle);
//    }
//
//    DataItemType& operator*(void)
//    {
//        return *_array->get(_handle);
//    }
//
//    void release(void)
//    {
//        _array->deallocate(_handle);
//        _handle = invalid_handle;
//    }
//
//private:
//    DataArrayHandle          _handle{ 0 };
//    DataArray<DataItemType>* _array{ nullptr };
//};

template<class DataItemType>
class DataArray
{
public:
    DataArray(size_t capacity, uint16_t unique_key)
        : _capacity(capacity)
    {
        set_unique_key(unique_key);
        _data = static_cast<DataItemType*>(malloc(sizeof(DataItemType) * capacity));
        _handles = static_cast<DataArrayHandle*>(malloc(sizeof(DataArrayHandle) * capacity));
        for(size_t i{ 0 }; i < capacity; ++i)
        {
            _handles[i] = invalid_handle;
        }
    }

    DataArray(void)
        : DataArray(_c_default_capacity, 0)
    {
    }

    ~DataArray(void)
    {
        free(_data);
        free(_handles);
    }

    // Set the unique key for a DataArray.
    // The unique key is encoded into all handles.
    // Thus handles will be as unique as your system of generating and using these provided keys.
    void set_unique_key(uint16_t unique_key)
    {
        assert(unique_key != 0 || "DataArray unique key must not be 0.");
        assert(_unique_key == 0 || "DataArray unique key cannot currently be re-set.");
        _unique_key = unique_key;
    }

    // Constructs an object in place and returns a handle reference to it.
    template<typename... Args>
    DataArrayHandle allocate(Args&&... args)
    {
        uint32_t idx = _next_idx();
        if (idx == invalid_handle)
        {
            return invalid_handle;
        }

        new (&_data[idx]) DataItemType(std::forward<Args>(args)...);
        _handles[idx] = _make_handle(idx);
        ++_count;

        return _handles[idx];
    }

    // Destructs the stored object and sets the passed handle to invalid.
    void deallocate(DataArrayHandle& handle)
    {
        uint64_t idx = _get_idx(handle);
        _data[idx].~DataItemType();

        if( _free_head != invalid_handle )
        {
            // There are some open nodes on the free list
            uint32_t free_head_idx = _get_idx(_free_head);
            _handles[idx] = _make_invalid_key_handle(free_head_idx);
            _free_head = _make_invalid_key_handle(idx);
        }
        else
        {
            // There are no open nodes on the free list
            _handles[idx] = invalid_handle;
            _free_head = _make_invalid_key_handle(idx);
        }

        --_count;
        handle = invalid_handle;
    }

    void clear(void)
    {
        for(int i{ 0 }; i < _count; ++i)
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
        return &_data[idx];
    }

    // Check that the key for the item matches the data
    bool check_valid(DataArrayHandle handle) const
    {
        uint64_t key = _get_key(handle);
        uint64_t idx = _get_idx(handle);

        if (key == key_mask || idx > _capacity)
        {
            return false;
        }

        const DataArrayHandle check_handle = _handles[idx];

        return _get_key(handle) == _get_key(check_handle);
    }

    // Get the raw array of data.
    DataItemType* data(void) const
    {
        return _data;
    }

    // Get the raw array of handles.
    DataArrayHandle* handles(void) const
    {
        return _handles;
    }

    DataArray(const DataArray&)            = delete;
    DataArray& operator=(const DataArray&) = delete;

    DataArray(DataArray&& other)
    {
        if (this != &other)
        {
            _data      = other._data;
            _handles   = other.handles;
            _capacity  = other._capacity;
            _max_used  = other._max_used;
            _count     = other._count;
            _next_key  = other._next_key;
            _free_head = other._free_head;

            other._data      = nullptr;
            other._handles   = nullptr;
            other._capacity  = 0;
            other._max_used  = 0;
            other._count     = 0;
            other._next_key  = 1;
            other._free_head = invalid_handle;
        }
    }

    DataArray& operator=(DataArray&& other) noexcept
    {
        if (this != &other)
        {
            _data      = other._data;
            _handles   = other._handles;
            _capacity  = other._capacity;
            _max_used  = other._max_used;
            _count     = other._count;
            _next_key  = other._next_key;
            _free_head = other._free_head;

            other._data      = nullptr;
            other._handles   = nullptr;
            other._capacity  = 0;
            other._max_used  = 0;
            other._count     = 0;
            other._next_key  = 1;
            other._free_head = invalid_handle;
        }

        return *this;
    }

private:
    inline DataArrayHandle _get_unique_key(DataArrayHandle handle) const
    {
        return handle & unique_key_mask;
    }

    inline DataArrayHandle _get_key(DataArrayHandle handle) const
    {
        return handle & key_mask;
    }

    inline DataArrayHandle _get_idx(DataArrayHandle handle) const
    {
        return handle & idx_mask;
    }

   // Construct a handle by encoding info into it
   inline DataArrayHandle _make_handle(uint32_t idx)
   {
       DataArrayHandle new_handle_unique_key{ static_cast<uint64_t>(_unique_key) << _c_unique_key_shift };
       DataArrayHandle new_handle_item_key{ static_cast<uint64_t>(_next_key++) << _c_item_key_shift };
       DataArrayHandle new_handle{ new_handle_unique_key | new_handle_item_key | static_cast<uint64_t>(idx) };
       return new_handle;
   }

   // Construct a handle that's invalid, but points towards a free index
   inline DataArrayHandle _make_invalid_key_handle(uint32_t idx) const
   {
       return (key_mask | idx);
   }

   inline uint32_t _next_idx(void)
   {
       uint64_t idx { invalid_handle };

        if (_free_head != invalid_handle)
        {
            // There are open nodes on the free list
            idx = _get_idx(_free_head);

            if( _handles[idx] != invalid_handle)
            {
                // There is an open node to set the free list head to
                uint32_t _next_free_head_idx = _get_idx(_handles[idx]);
                _free_head = _handles[_next_free_head_idx];
            }
            else
            {
                // There are no open nodes to set the free list head to
                _free_head = invalid_handle;
            }
        }
        else
        {
            // There are only closed nodes on the free list, open one up
            idx = _max_used;
            ++_max_used;
        }

        return idx;
   }

/* Data */
private:
    DataItemType*    _data{ nullptr };
    DataArrayHandle* _handles{ nullptr };
    size_t           _capacity{ 0 };
    uint32_t         _max_used{ 0 };
    uint32_t         _count{ 0 };
    uint16_t         _next_key{ 1 };
    uint16_t         _unique_key{ 0 };
    DataArrayHandle  _free_head { invalid_handle };

    static const size_t _c_default_capacity{ 1024 };
    static const int    _c_unique_key_shift{ 48 };
    static const int    _c_item_key_shift{ 32 };

/* ITERATOR */
public:

    class iterator
    {
    public:
        iterator(DataArray<DataItemType>& container, uint32_t start)
            : _container(container),
              _current(start)
        {
            // Seek first valid element
            while (_current != _container._max_used)
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
        iterator operator++(void)
        {
            // Using max used instead of count or capacity because we operate a free list within a chunk of memory.
            // All we know is that _max_used have been allocated and MAY be valid. No need to go beyond that.
            while (_current != _container._max_used)
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

        bool operator!=(const iterator& other)
        {
            return _current != other._current;
        }

        const DataArrayHandle& operator*(void)
        {
            return _container.handles()[_current];
        }

    private:
        DataArray<DataItemType>& _container;
        size_t                   _current{ 0 };
    };

    iterator begin(void) { return iterator(*this, 0); }
    iterator end(void)   { return iterator(*this, _max_used); }
};

//template<class DataItemType>
//DataAccessor<DataItemType> make_accessor(DataArray<DataItemType>& array)
//{
//    return { array.allocate(), array };
//}

}

#endif
