#include "semaphore.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

#include <cassert>

using namespace rend;

StatusCode Semaphore::create(void)
{
    assert(_vk_semaphore == VK_NULL_HANDLE && "Attempt to create a Semaphore that has already been created.");

    VkSemaphoreCreateInfo create_info = vulkan_helpers::gen_semaphore_create_info();

    _vk_semaphore = static_cast<VulkanDeviceContext&>(DeviceContext::instance()).get_device()->create_semaphore(create_info);

    if(_vk_semaphore == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

void Semaphore::destroy(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_semaphore(_vk_semaphore);
    _vk_semaphore = VK_NULL_HANDLE;
}

VkSemaphore Semaphore::get_handle(void) const
{
    return _vk_semaphore;
}
