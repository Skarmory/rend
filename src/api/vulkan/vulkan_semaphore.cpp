#include "api/vulkan/vulkan_semaphore.h"

#include "core/device_context.h"
#include "core/rend_service.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_device_context.h"

#include <cassert>

using namespace rend;

Semaphore::Semaphore(void)
{
    VkSemaphoreCreateInfo create_info = vulkan_helpers::gen_semaphore_create_info();
    _vk_semaphore = static_cast<VulkanDeviceContext&>(*RendService::device_context()).create_semaphore(create_info);
}

Semaphore::~Semaphore(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());
    ctx.destroy_semaphore(_vk_semaphore);
}

VkSemaphore Semaphore::handle(void) const
{
    return _vk_semaphore;
}
