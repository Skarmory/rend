#ifndef REND_SEMAPHORE_H
#define REND_SEMAPHORE_H

#include "core/rend_defs.h"

#include <vulkan.h>

namespace rend
{

class Semaphore
{
public:
    Semaphore(void)                        = default;
    ~Semaphore(void)                       = default;
    Semaphore(const Semaphore&)            = delete;
    Semaphore(Semaphore&&)                 = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&)      = delete;

    StatusCode create(void);
    void       destroy(void);

    VkSemaphore get_handle(void) const;

private:
    VkSemaphore _vk_semaphore { VK_NULL_HANDLE };
};

}

#endif
