#include "event.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

Event::Event(LogicalDevice* device) : _device(device)
{
    VkEventCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    VULKAN_DEATH_CHECK(
        vkCreateEvent(_device->get_handle(), &create_info, nullptr, &_vk_event),
        "Failed to create event"
    );
}

Event::~Event(void)
{
    vkDestroyEvent(_device->get_handle(), _vk_event, nullptr);
}

VkEvent Event::get_handle(void) const
{
    return _vk_event;
}
