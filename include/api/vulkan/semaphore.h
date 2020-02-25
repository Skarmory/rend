#ifndef REND_SEMAPHORE_H
#define REND_SEMAPHORE_H

#include "rend_defs.h"

#include <vulkan.h>

namespace rend
{

class Semaphore
{
public:
    Semaphore(void) = default;
    ~Semaphore(void);
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&)      = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&)      = delete;

    StatusCode create_semaphore(void);

    VkSemaphore get_handle(void) const;

private:
    VkSemaphore _vk_semaphore { VK_NULL_HANDLE };
};

}

#endif
