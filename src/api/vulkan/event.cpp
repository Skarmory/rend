#include "event.h"

#include "device_context.h"
#include "logical_device.h"
#include "utils.h"

using namespace rend;

Event::Event(DeviceContext* context) : _context(context)
{
}

Event::~Event(void)
{
    vkDestroyEvent(_context->get_device()->get_handle(), _vk_event, nullptr);
}

bool Event::create_event(void)
{
    VkEventCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    if(vkCreateEvent(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_event) != VK_SUCCESS)
    {
        std::cerr << "Failed to create event" << std::endl;
        return false;
    }

    return true;
}

VkEvent Event::get_handle(void) const
{
    return _vk_event;
}
