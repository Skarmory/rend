#ifndef REND_FENCE_H
#define REND_FENCE_H

#include <vulkan.h>
#include <limits>

namespace rend
{

class DeviceContext;

class Fence
{
public:
    explicit Fence(DeviceContext& context);
    ~Fence(void);

    Fence(const Fence&)            = delete;
    Fence(Fence&&)                 = delete;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&)      = delete;

    bool create_fence(bool start_signalled);

    VkFence  get_handle(void) const;
    void     reset(void) const;
    VkResult wait(uint64_t timeout=std::numeric_limits<uint64_t>::max()) const;

private:
    DeviceContext& _context;
    VkFence        _vk_fence;
};

}

#endif
