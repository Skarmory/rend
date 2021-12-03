#include "api/vulkan/event.h"

#include "core/device_context.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_device_context.h"

#include <cassert>

using namespace rend;

bool Event::create(void)
{
    assert(_vk_event == VK_NULL_HANDLE && "Attempt to create an Event that has already been created.");

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
    _vk_event = VK_NULL_HANDLE;
}

VkEvent Event::get_handle(void) const
{
    return _vk_event;
}
