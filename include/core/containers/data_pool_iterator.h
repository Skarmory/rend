#ifndef REND_CORE_CONTAINER_DATA_POOL_ITERATOR_H
#define REND_CORE_CONTAINER_DATA_POOL_ITERATOR_H

namespace rend
{

template<class T>
class DataPoolIterator
{
public:
    DataPoolIterator(DataArrayBase& container, uint32_t start)
        : _container(container),
          _current(start)
    {
    }

    // Seek next valid element
    DataPoolIterator<T>& operator++(void)
    {
        ++_current;
        return *this;
    }

    bool operator!=(const DataPoolIterator& other) const
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
class DataPoolConstIterator
{
public:
    DataPoolConstIterator(const DataArrayBase& container, uint32_t start)
        : _container(container),
          _current(start)
    {
    }

    // Seek next valid element
    DataPoolConstIterator<T>& operator++(void)
    {
        ++_current;
        return *this;
    }

    bool operator!=(const DataPoolConstIterator& other) const
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
