#ifndef REND_DATA_ARRAY_BASE_H
#define REND_DATA_ARRAY_BASE_H

namespace rend
{

typedef uint64_t DataArrayHandle;

static const DataArrayHandle c_index_mask{ 0xffffffff };
static const DataArrayHandle c_generation_mask{ 0x7ffffff00000000 };
static const DataArrayHandle c_invalid_mask   { 0x80000000 };
static const size_t          c_default_capacity{ 1024 };
static const int             c_generation_shift{ 32 };

static const DataArrayHandle invalid_handle { c_invalid_mask | c_generation_mask | c_index_mask };

constexpr bool is_invalid_handle(DataArrayHandle handle) { return (handle & c_invalid_mask) != 0; }

class DataArrayBase
{

public:
    DataArrayBase(size_t item_size, size_t capacity);
    DataArrayBase(void);
    DataArrayBase(const DataArrayBase&)            = delete;
    DataArrayBase& operator=(const DataArrayBase&) = delete;
    DataArrayBase(DataArrayBase&& other);
    DataArrayBase& operator=(DataArrayBase&& other) noexcept;
    virtual ~DataArrayBase(void);

    void*            get(DataArrayHandle handle) const;
    bool             check_valid(DataArrayHandle handle) const;
    void*            data(void) const;
    DataArrayHandle* handles(void) const;
    uint32_t         size(void) const;
    uint32_t         max_used(void) const;

protected:
    DataArrayHandle _get_gen(DataArrayHandle handle) const;
    DataArrayHandle _get_idx(DataArrayHandle handle) const;
    DataArrayHandle _make_handle(uint32_t generation, uint32_t idx);
    DataArrayHandle _make_invalid_handle(uint32_t generation, uint32_t idx) const;
    uint32_t        _next_idx(void);

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
