#ifndef REND_CORE_CONTAINERS_DATA_POOL_H
#define REND_CORE_CONTAINERS_DATA_POOL_H

#include "core/alloc/allocator.h"
#include "core/containers/data_array_base.h"
#include "core/containers/data_pool_iterator.h"

namespace rend
{

template<typename DataItemType, size_t N, class AllocatorType=Allocator<DataItemType>>
class DataPool : public DataArrayBase
{
    public:
        DataPool(void)
            :
                DataArrayBase(sizeof(DataItemType), N)
        {
        }

        ~DataPool(void)
        {
            for(size_t i = 0; i < N; ++i)
            {
                DataArrayHandle handle = _handles[i];
                if(!is_invalid_handle(handle))
                {
                    _allocator.destruct(&static_cast<DataItemType*>(_data)[i]);
                }
            }
        }

        template<typename... Args>
        void initialise(Args&&... args)
        {
            for(size_t i = 0; i < N; ++i)
            {
                DataArrayHandle handle = _allocate();
                _allocator.construct(&static_cast<DataItemType*>(_data)[_get_idx(handle)], std::forward<Args>(args)...);
            }

            for(size_t i = 0; i < N; ++i)
            {
                release(_handles[i]);
            }
        }

        void release(DataArrayHandle handle)
        {
            uint64_t idx = _get_idx(handle);
            uint64_t gen = _get_gen(handle);

            _deallocate(idx, gen);
        }

        DataArrayHandle acquire(void)
        {
            DataArrayHandle handle = _allocate();
            return handle;
        }

        DataItemType* get(DataArrayHandle handle) const
        {
            if(!check_valid(handle))
            {
                return nullptr;
            }

            return &(static_cast<DataItemType*>(_data)[_get_idx(handle)]);
        }

        DataPoolIterator<DataItemType> begin(void) { return DataPoolIterator<DataItemType>(*this, 0); }
        DataPoolIterator<DataItemType> end(void)   { return DataPoolIterator<DataItemType>(*this, _max_used); }

        DataPoolConstIterator<DataItemType> begin(void) const { return DataPoolConstIterator<DataItemType>(*this, 0); }
        DataPoolConstIterator<DataItemType> end(void)   const { return DataPoolConstIterator<DataItemType>(*this, _max_used); }

    private:
        AllocatorType _allocator;
};

}

#endif
