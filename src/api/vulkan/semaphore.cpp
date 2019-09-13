#include "semaphore.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

Semaphore::Semaphore(LogicalDevice* device) : _device(device)
{
    VkSemaphoreCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    VULKAN_DEATH_CHECK(vkCreateSemaphore(_device->get_handle(), &create_info, nullptr, &_vk_semaphore), "Failed to create semaphore");
}

Semaphore::~Semaphore(void)
{
    vkDestroySemaphore(_device->get_handle(), _vk_semaphore, nullptr);
}

VkSemaphore Semaphore::get_handle(void) const
{
    return _vk_semaphore;
}
