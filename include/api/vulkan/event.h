#ifndef REND_EVENT_H
#define REND_EVENT_H

#include <vulkan.h>

namespace rend
{

class Event
{
public:
    explicit Event(void);
    ~Event(void);
    Event(const Event&)            = delete;
    Event(Event&&)                 = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&)      = delete;

    VkEvent handle(void) const;

private:
    VkEvent _vk_event { VK_NULL_HANDLE };
};

}

#endif
