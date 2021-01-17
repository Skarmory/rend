#include "event.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

using namespace rend;

bool Event::create(void)
{
    if(_vk_event != VK_NULL_HANDLE)
    {
        return false;
    }

    VkEventCreateInfo create_info = vulkan_helpers::gen_event_create_info();

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_event = ctx.get_device()->create_event(create_info);
    if(_vk_event == VK_NULL_HANDLE)
    {
        return false;
    }

    return true;
}

void Event::destroy(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_event(_vk_event);
}

VkEvent Event::get_handle(void) const
{
    return _vk_event;
}
