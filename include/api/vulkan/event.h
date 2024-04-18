#ifndef REND_API_VULKAN_EVENT_H
#define REND_API_VULKAN_EVENT_H

#include <vulkan.h>

namespace rend
{

class VulkanDeviceContext;

class Event
{
public:
    explicit Event(VulkanDeviceContext& ctx);
    ~Event(void);
    Event(const Event&)            = delete;
    Event(Event&&)                 = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&)      = delete;

    VkEvent vk_handle(void) const;

private:
    VkEvent _vk_event { VK_NULL_HANDLE };
    VulkanDeviceContext* _ctx{ nullptr };
};

}

#endif
