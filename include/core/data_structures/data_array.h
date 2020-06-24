#ifndef REND_DATA_ARRAY_H
#define REND_DATA_ARRAY_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace rend
{

typedef uint32_t DataArrayHandle;

static const DataArrayHandle invalid_handle { UINT_MAX };

template<class DataItemType>
class DataArray;

template<class DataItemType>
class DataAccessor
{
public:
    DataAccessor(void) {}
    DataAccessor(DataArrayHandle handle, DataArray<DataItemType>& array)
        : _handle(handle)
        , _array(&array)
    {
    }

    DataItemType* operator->(void)
    {
        return _array->get(_handle);
    }

    DataItemType& operator*(void)
    {
        return *_array->get(_handle);
    }

    void release(void)
    {
        _array->deallocate(_handle);
        _handle = invalid_handle;
    }

private:
    DataArrayHandle          _handle{ 0 };
    DataArray<DataItemType>* _array{ nullptr };
};

template<class DataItemType>
class DataArray
{
public:
    DataArray(size_t capacity)
        : _capacity(capacity)
    {
        _items = static_cast<DataArrayItem*>(malloc(sizeof(DataArrayItem) * capacity));
    }

    DataArray(void)
        : DataArray(_c_default_capacity)
    {
    }

    ~DataArray(void)
    {
        free(_items);
    }

    template<typename... Args>
    DataArrayHandle allocate(Args&&... args)
    {
        uint32_t idx = _next_idx();
        if (idx == invalid_handle)
        {
            return invalid_handle;
        }

        DataArrayItem& item = _items[idx];
        new (&item.data) DataItemType(std::forward<Args>(args)...);
        item.handle = _make_handle(idx);
        ++_count;

        return item.handle;
    }

    void deallocate(DataArrayHandle& handle)
    {
        uint32_t key = _get_key(handle);
        uint32_t idx = _get_idx(handle);

        DataArrayItem& item = _items[idx];
        if (key != _get_key(item.handle))
        {
            return;
        }

        item.data.~DataItemType();

        item.handle = _free_head;
        _free_head = idx;

        --_count;
        handle = invalid_handle;
    }

    DataItemType* get(DataArrayHandle handle) const
    {
        uint32_t key = _get_key(handle);
        uint32_t idx = _get_idx(handle);

        DataArrayItem& item = _items[idx];

        if (key != _get_key(item.handle))
        {
            return nullptr;
        }

        return &item.data;
    }

    bool check_valid(DataArrayHandle handle) const
    {
        uint32_t idx = _get_idx(handle);
        if (idx > _capacity)
        {
            return false;
        }

        const auto& item = _items[idx];

        return _get_key(handle) == _get_key(item.handle);
    }

    DataArray(const DataArray&)            = delete;
    DataArray& operator=(const DataArray&) = delete;

    DataArray(DataArray&& other)
    {
        if (this != &other)
        {
            _items     = other._items;
            _capacity  = other._capacity;
            _max_used  = other._max_used;
            _count     = other._count;
            _next_key  = other._next_key;
            _free_head = other._free_head;

            other._items     = nullptr;
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
            _items     = other._items;
            _capacity  = other._capacity;
            _max_used  = other._max_used;
            _count     = other._count;
            _next_key  = other._next_key;
            _free_head = other._free_head;

            other._items     = nullptr;
            other._capacity  = 0;
            other._max_used  = 0;
            other._count     = 0;
            other._next_key  = 1;
            other._free_head = invalid_handle;
        }

        return *this;
    }

private:
   inline DataArrayHandle _get_key(DataArrayHandle handle) const { return handle & key_mask; }
   inline DataArrayHandle _get_idx(DataArrayHandle handle) const { return handle & idx_mask; }
   inline DataArrayHandle _make_handle(uint32_t idx)             { return (_next_key++ << 16 | idx); }

   inline uint32_t _next_idx(void)
   {
       uint32_t idx { invalid_handle };

        if (_free_head != invalid_handle)
        {
            idx = _free_head;
            _free_head = _get_idx(_items[idx].handle);
        }
        else
        {
            idx = _max_used;
            ++_max_used;
        }

        return idx;
   }

/* Data */
public:
    static const DataArrayHandle idx_mask       { 0xffff };
    static const DataArrayHandle key_mask       { 0xffff0000 };

private:
    struct DataArrayItem
    {
        DataItemType    data;
        DataArrayHandle handle { invalid_handle };
    };

    DataArrayItem*  _items     { nullptr };
    size_t          _capacity  { 0 };
    uint32_t        _max_used  { 0 };
    uint32_t        _count     { 0 };
    uint32_t        _next_key  { 1 };
    DataArrayHandle _free_head { invalid_handle };

    static const size_t   _c_default_capacity { 1024 };

/* ITERATOR */
public:

    class iterator
    {
    public:
        iterator(DataArrayItem* items) : _items(items) {}

        iterator operator++(void)
        {
            ++_items;
            return *this;
        }

        bool operator!=(const iterator& other)
        {
            return _items != other._items;
        }

        const DataArrayHandle& operator*(void)
        {
            return _items->handle;
        }

    private:
        DataArrayItem* _items{ nullptr };
    };

    iterator begin(void) { return iterator(_items); }
    iterator end(void)   { return iterator(_items + _capacity); }
};

template<class DataItemType>
DataAccessor<DataItemType> make_accessor(DataArray<DataItemType>& array)
{
    return { array.allocate(), array };
}

}

#endif
