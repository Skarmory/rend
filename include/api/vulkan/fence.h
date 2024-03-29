#ifndef REND_FENCE_H
#define REND_FENCE_H

#include <vulkan.h>
#include <limits>

namespace rend
{

class Fence
{
public:
    explicit Fence(bool start_signalled);
    Fence(void);
    ~Fence(void);
    Fence(const Fence&)            = delete;
    Fence(Fence&&)                 = default;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&)      = default;

    VkFence  handle(void) const;
    void     reset(void) const;
    VkResult wait(uint64_t timeout=std::numeric_limits<uint64_t>::max()) const;

private:
    VkFence _vk_fence { VK_NULL_HANDLE };
};

}

#endif
