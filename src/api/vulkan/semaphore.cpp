#include "semaphore.h"

#include "device_context.h"
#include "logical_device.h"

using namespace rend;

Semaphore::Semaphore(DeviceContext& context)
    : _context(context),
      _vk_semaphore(VK_NULL_HANDLE)
{
}

Semaphore::~Semaphore(void)
{
    _context.get_device()->destroy_semaphore(_vk_semaphore);
}

StatusCode Semaphore::create_semaphore(void)
{
    if(_vk_semaphore != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    _vk_semaphore = _context.get_device()->create_semaphore();

    if(_vk_semaphore == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

VkSemaphore Semaphore::get_handle(void) const
{
    return _vk_semaphore;
}
