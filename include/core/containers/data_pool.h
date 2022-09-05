#ifndef REND_CORE_CONTAINERS_DATA_POOL_H
#define REND_CORE_CONTAINERS_DATA_POOL_H

#include "core/alloc/allocator.h"

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
                _allocator.destruct(&static_cast<DataItemType*>(_data)[i]);
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

            return &static_cast<DataItemType*>(_data)[_get_idx(handle)];
        }

    private:
        AllocatorType _allocator;
};

}

#endif
