#ifndef REND_FENCE_H
#define REND_FENCE_H

#include <vulkan.h>
#include <limits>

namespace rend
{

class Fence
{
public:
    Fence(void)                    = default;
    ~Fence(void)                   = default;
    Fence(const Fence&)            = delete;
    Fence(Fence&&)                 = delete;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&)      = delete;

    bool create(bool start_signalled);
    void destroy(void);

    VkFence  get_handle(void) const;
    void     reset(void) const;
    VkResult wait(uint64_t timeout=std::numeric_limits<uint64_t>::max()) const;

private:
    VkFence _vk_fence { VK_NULL_HANDLE };
};

}

#endif
