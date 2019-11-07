#include "event.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

Event::Event(DeviceContext& context)
    : _context(context),
      _vk_event(VK_NULL_HANDLE)
{
}

Event::~Event(void)
{
    _context.get_device()->destroy_event(_vk_event);
}

bool Event::create_event(void)
{
    if(_vk_event != VK_NULL_HANDLE)
        return false;

    VkEventCreateInfo create_info = vulkan_helpers::gen_event_create_info();

    _vk_event = _context.get_device()->create_event(create_info);
    if(_vk_event == VK_NULL_HANDLE)
        return false;

    return true;
}

VkEvent Event::get_handle(void) const
{
    return _vk_event;
}
