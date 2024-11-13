#ifndef CORE_CONTAINERS_REND_RING_BUFFER_H
#define CORE_CONTAINERS_REND_RING_BUFFER_H

#include <cstdint>
#include <cstdlib>
#include <utility>

namespace rend
{

template<class T>
class RingBuffer
{
    public:
        RingBuffer(void) : RingBuffer(_C_DEFAULT_CAPACITY) {}
        RingBuffer(uint32_t capacity)
            :
                _count(0),
                _capacity(capacity)
        {
            _data = (unsigned char*)malloc(sizeof(T) * capacity);
        }

        ~RingBuffer(void)
        {
            free(_data);
        }
        
        // Copy
        RingBuffer(const RingBuffer& other)
        {
            _data = other.data;
            _current_idx = other._current_idx;
            _count = other._count;
            _capacity = other._capacity;
        }

        RingBuffer(const RingBuffer&& other)
        {
            if(&other != this)
            {
                _data = other.data;
                _current_idx = other._current_idx;
                _count = other._count;
                _capacity = other._capacity;

                other.data = nullptr;
                other._current_idx = 0;
                other._count = 0;
                other.capacity = 0;
            }
        }

        // Move
        T& operator=(const RingBuffer& other)
        {
            _data = other.data;
            _current_idx = other._current_idx;
            _count = other._count;
            _capacity = other._capacity;
        }

        T& operator=(const RingBuffer&& other)
        {
            if(&other != this)
            {
                _data = other.data;
                _current_idx = other._current_idx;
                _count = other._count;
                _capacity = other._capacity;

                other.data = nullptr;
                other._current_idx = 0;
                other._count = 0;
                other.capacity = 0;
            }
        }

        // Mutators

        void add(const T& item)
        {
            _grow();
            ((T*)_data)[_count++] = item;
        }

        void clear(void)
        {
            _count = 0;
        }

        template<class... Args>
        T& emplace(Args&&... args)
        {
            _grow();
            return *new (&static_cast<T*>(_data)[_count++]) T(std::forward<Args>(args)...);
        }

        // Accessors

        uint32_t count(void) const
        {
            return _count;
        }

        const T& get(void) const
        {
            return ((const T*)_data)[_current_idx];
        }

        T& get(void)
        {
            return const_cast<T&>(std::as_const(*this).get());
        }

        T& next(void)
        {
            _current_idx = (_current_idx + 1) % _count;
            return get();
        }

    private /*funcs*/:
        void _grow(void)
        {
            if(_count == _capacity)
            {
                _capacity <<= 1;
                _data = (unsigned char*)realloc(_data, sizeof(T) * _capacity);
            }
        }

    private /*data*/:
        unsigned char* _data{ nullptr };
        uint32_t _current_idx{ 0 };
        uint32_t _count{ 0 };
        uint32_t _capacity{ 0 };
        static const uint32_t _C_DEFAULT_CAPACITY{ 8 };
};

}

#endif
