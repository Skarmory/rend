#ifndef REND_DATA_ARRAY_H
#define REND_DATA_ARRAY_H

#include <climits>
#include <cstddef>
#include <cstdint>

namespace rend::core
{

typedef uint32_t DataArrayHandle;

template<class DataItemType>
class DataArray
{
public:
    DataArray(size_t capacity)
        : _capacity(capacity)
    {
        _items = new DataArrayItem[capacity];
    }

    DataArray(void)
        : DataArray(_c_default_capacity)
    {
    }

    ~DataArray(void)
    {
        delete[] _items;
    }

    DataArrayHandle allocate(void)
    {
        uint32_t idx = _next_idx();
        if( idx == invalid_handle)
        {
            return invalid_handle;
        }

        DataArrayItem& item = _items[idx];
        item.handle = _make_handle(idx);
        ++_count;

        return item.handle;
    }

    void deallocate(DataArrayHandle& handle)
    {
        uint32_t key = _get_key(handle);
        uint32_t idx = _get_idx(handle);

        DataArrayItem& item = _items[idx];
        if(key != _get_key(item.handle))
        {
            return;
        }

        item.handle = _free_head;
        _free_head = idx;

        --_count;
        handle = invalid_handle;
    }

    DataItemType* get(DataArrayHandle handle)
    {
        uint32_t key = _get_key(handle);
        uint32_t idx = _get_idx(handle);

        DataArrayItem& item = _items[idx];

        if(key != _get_key(item.handle))
        {
            return nullptr;
        }

        return &item.data;
    }

    DataArray(const DataArray&)            = delete;
    DataArray(DataArray&&)                 = delete;
    DataArray& operator=(const DataArray&) = delete;
    DataArray& operator=(DataArray&&)      = delete;

private:
   inline DataArrayHandle _get_key(DataArrayHandle handle) const { return handle & key_mask; }
   inline DataArrayHandle _get_idx(DataArrayHandle handle) const { return handle & idx_mask; }
   inline DataArrayHandle _make_handle(uint32_t idx)             { return (_next_key++ << 16 | idx); }

   inline uint32_t _next_idx(void)
   {
       uint32_t idx { invalid_handle };

        if(_free_head != invalid_handle)
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
    static const DataArrayHandle invalid_handle { UINT_MAX };
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
};

}

#endif
