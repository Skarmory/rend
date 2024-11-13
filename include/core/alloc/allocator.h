#ifndef REND_CORE_ALLOC_ALLOCATOR_H
#define REND_CORE_ALLOC_ALLOCATOR_H

#include <utility>

namespace rend
{

class Renderer;
class VulkanBuffer;
class VulkanDeviceContext;
class VulkanRenderer;
struct BufferInfo;

template<class T>
class Allocator
{
    public:
        template<typename... Args>
        [[nodiscard]]
        T* allocate(Args&&... args)
        {
            return new T(std::forward<Args>(args)...);
        }

        void deallocate(T* obj)
        {
            delete obj;
        }

        template<typename... Args>
        void construct(T* p, Args&&... args)
        {
            new (p) T(std::forward<Args>(args)...);
        }

        void destruct(T* p)
        {
            p->~T();
        }
};

}

#endif
