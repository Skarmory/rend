#ifndef REND_SEMAPHORE_H
#define REND_SEMAPHORE_H

#include <vulkan.h>

namespace rend
{

class DeviceContext;

class Semaphore
{
public:
    Semaphore(DeviceContext* context);
    ~Semaphore(void);
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&)      = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&)      = delete;

    bool create_semaphore(void);

    VkSemaphore get_handle(void) const;

private:
    VkSemaphore _vk_semaphore;

    DeviceContext* _context;
};

}

#endif
