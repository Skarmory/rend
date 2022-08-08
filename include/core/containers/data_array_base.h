#ifndef REND_DATA_ARRAY_BASE_H
#define REND_DATA_ARRAY_BASE_H

#include <cstdint>
#include <cstddef>

namespace rend
{

typedef uint64_t DataArrayHandle;

static const DataArrayHandle c_index_mask{      0x00000000ffffffff };
static const DataArrayHandle c_generation_mask{ 0x7fffffff00000000 };
static const DataArrayHandle c_invalid_mask   { 0x8000000000000000 };
static const size_t          c_default_capacity{ 1024 };
static const int             c_generation_shift{ 32 };

static const DataArrayHandle invalid_handle { c_invalid_mask | c_generation_mask | c_index_mask };

constexpr bool is_invalid_handle(DataArrayHandle handle) { return (handle & c_invalid_mask) != 0; }

class DataArrayBase
{

public:
    DataArrayBase(size_t item_size, size_t capacity);
    DataArrayBase(const DataArrayBase&)            = delete;
    DataArrayBase& operator=(const DataArrayBase&) = delete;
    DataArrayBase(DataArrayBase&& other);
    DataArrayBase& operator=(DataArrayBase&& other) noexcept;
    virtual ~DataArrayBase(void);

    bool             check_valid(DataArrayHandle handle) const;
    void*            data(void) const;
    DataArrayHandle* handles(void) const;
    uint32_t         size(void) const;
    uint32_t         max_used(void) const;

protected:
    uint64_t        _get_gen(DataArrayHandle handle) const;
    uint64_t        _get_idx(DataArrayHandle handle) const;
    DataArrayHandle _make_handle(uint64_t generation, uint64_t idx) const;
    DataArrayHandle _make_invalid_handle(uint64_t generation, uint64_t idx) const;
    DataArrayHandle _allocate(void);
    void            _deallocate(uint64_t idx, uint64_t gen);

/* Data */
protected:
    void*            _data{ nullptr };
    DataArrayHandle* _handles{ nullptr };
    size_t           _capacity{ 0 };
    uint32_t         _max_used{ 0 };
    uint32_t         _count{ 0 };
    DataArrayHandle  _free_head { invalid_handle };

};

}

#endif
