#ifndef REND_EVENT_H
#define REND_EVENT_H

#include <vulkan.h>

namespace rend
{

class DeviceContext;

class Event
{
public:
    Event(DeviceContext* context);
    ~Event(void);
    Event(const Event&) = delete;
    Event(Event&&)      = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&)      = delete;

    bool create_event(void);

    VkEvent get_handle(void) const;

private:
    VkEvent _vk_event;

    DeviceContext* _context;
};

}

#endif
