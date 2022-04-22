#include "api/vulkan/event.h"

#include "core/device_context.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_device_context.h"

using namespace rend;

Event::Event(void)
{
    VkEventCreateInfo create_info = vulkan_helpers::gen_event_create_info();

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_event = ctx.create_event(create_info);
}

Event::~Event(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.destroy_event(_vk_event);
}

VkEvent Event::handle(void) const
{
    return _vk_event;
}
