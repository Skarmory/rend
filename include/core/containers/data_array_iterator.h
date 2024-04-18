#ifndef REND_CORE_CONTAINER_DATA_ARRAY_ITERATOR_H
#define REND_CORE_CONTAINER_DATA_ARRAY_ITERATOR_H

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

}

#endif
