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
#include "core/containers/data_array_iterator.h"

namespace rend
{

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

    DataArrayIterator<DataItemType> begin(void) const { return DataArrayIterator<DataItemType>(*this, 0); }
    DataArrayIterator<DataItemType> end(void) const { return DataArrayIterator<DataItemType>(*this, _count); }

    DataArrayConstIterator<DataItemType> cbegin(void) const { return DataArrayConstIterator<DataItemType>(*this, 0); }
    DataArrayConstIterator<DataItemType> cend(void)   const { return DataArrayConstIterator<DataItemType>(*this, _count); }

private:
    AllocatorType _allocator;
};

}

#endif
