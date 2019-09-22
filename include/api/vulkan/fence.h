#ifndef FENCE_H
#define FENCE_H

#include <vulkan.h>
#include <limits>

namespace rend
{

class LogicalDevice;

class Fence
{
    friend class LogicalDevice;

public:
    Fence(const Fence&) = delete;
    Fence(Fence&&)      = delete;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&)      = delete;

    VkFence  get_handle(void) const;
    void     reset(void) const;
    VkResult wait(uint64_t timeout=std::numeric_limits<uint64_t>::max()) const;

private:
    Fence(LogicalDevice* device, bool start_signalled);
    ~Fence(void);

private:
    VkFence _vk_fence;

    LogicalDevice* _device;
};

}

#endif
