#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <vulkan/vulkan.h>

namespace rend
{

class LogicalDevice;

class Semaphore
{
    friend class LogicalDevice;

public:
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&)      = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&)      = delete;

    VkSemaphore get_handle(void) const;

private:
    Semaphore(LogicalDevice* device);
    ~Semaphore(void);

private:
    VkSemaphore _vk_semaphore;

    LogicalDevice* _device;
};

}

#endif
