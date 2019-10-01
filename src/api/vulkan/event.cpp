#include "event.h"

#include "device_context.h"
#include "logical_device.h"

using namespace rend;

Event::Event(DeviceContext* context)
    : _context(context),
      _vk_event(VK_NULL_HANDLE)
{
}

Event::~Event(void)
{
    vkDestroyEvent(_context->get_device()->get_handle(), _vk_event, nullptr);
}

bool Event::create_event(void)
{
    if(_vk_event != VK_NULL_HANDLE)
        return false;

    VkEventCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    if(vkCreateEvent(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_event) != VK_SUCCESS)
        return false;

    return true;
}

VkEvent Event::get_handle(void) const
{
    return _vk_event;
}
