#include "semaphore.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

Semaphore::~Semaphore(void)
{
    DeviceContext::instance().get_device()->destroy_semaphore(_vk_semaphore);
}

StatusCode Semaphore::create_semaphore(void)
{
    if(_vk_semaphore != VK_NULL_HANDLE)
    {
        return StatusCode::ALREADY_CREATED;
    }

    VkSemaphoreCreateInfo create_info = vulkan_helpers::gen_semaphore_create_info();

    _vk_semaphore = DeviceContext::instance().get_device()->create_semaphore(create_info);

    if(_vk_semaphore == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

VkSemaphore Semaphore::get_handle(void) const
{
    return _vk_semaphore;
}
