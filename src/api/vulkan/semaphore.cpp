#include "semaphore.h"

#include "device_context.h"
#include "logical_device.h"
#include "utils.h"

using namespace rend;

Semaphore::Semaphore(DeviceContext* context) : _context(context)
{
}

Semaphore::~Semaphore(void)
{
    vkDestroySemaphore(_context->get_device()->get_handle(), _vk_semaphore, nullptr);
}

bool Semaphore::create_semaphore(void)
{
    VkSemaphoreCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    if(vkCreateSemaphore(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_semaphore) != VK_SUCCESS)
        return false;

    return true;
}

VkSemaphore Semaphore::get_handle(void) const
{
    return _vk_semaphore;
}
