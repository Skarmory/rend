#ifndef REND_CORE_REND_OBJECT_H
#define REND_CORE_REND_OBJECT_H

#include <cstdint>
#include <limits>

namespace rend
{

typedef uint64_t RendHandle;
constexpr RendHandle REND_NULL_HANDLE = std::numeric_limits<uint64_t>::max();

class RendObject
{
    friend class Renderer;
    friend class VulkanRenderer;

    public:
        virtual ~RendObject(void) = default;
        RendHandle rend_handle(void) const { return _rend_handle; }

    protected:
        RendHandle _rend_handle{ REND_NULL_HANDLE };
};

}

#endif
