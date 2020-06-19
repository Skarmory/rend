#include "event.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

using namespace rend;

Event::~Event(void)
{
    static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->destroy_event(_vk_event);
}

bool Event::create_event(void)
{
    if(_vk_event != VK_NULL_HANDLE)
    {
        return false;
    }

    VkEventCreateInfo create_info = vulkan_helpers::gen_event_create_info();

    _vk_event = static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->create_event(create_info);
    if(_vk_event == VK_NULL_HANDLE)
    {
        return false;
    }

    return true;
}

VkEvent Event::get_handle(void) const
{
    return _vk_event;
}
