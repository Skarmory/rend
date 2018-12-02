#ifndef FENCE_H
#define FENCE_H

#include <vulkan/vulkan.h>

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

    VkFence get_handle(void) const;

private:
    Fence(LogicalDevice* device);
    ~Fence(void);

private:
    VkFence _vk_fence;

    LogicalDevice* _device;
};

}

#endif
