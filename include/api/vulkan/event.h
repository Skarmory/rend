#ifndef EVENT_H
#define EVENT_H

#include <vulkan.h>

namespace rend
{

class LogicalDevice;

class Event
{
    friend class LogicalDevice;

public:
    Event(const Event&) = delete;
    Event(Event&&)      = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&)      = delete;

    VkEvent get_handle(void) const;

private:
    Event(LogicalDevice* device);
    ~Event(void);

private:
    VkEvent _vk_event;

    LogicalDevice* _device;
};

}

#endif
