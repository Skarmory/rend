#include "api/vulkan/vulkan_semaphore.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"

using namespace rend;

Semaphore::Semaphore(VulkanDeviceContext& ctx, VkPipelineStageFlags wait_stages)
    :
        _wait_stages(wait_stages),
        _ctx(&ctx)
{
    VkSemaphoreCreateInfo create_info = vulkan_helpers::gen_semaphore_create_info();
    _vk_semaphore = _ctx->create_semaphore(create_info);
}

Semaphore::~Semaphore(void)
{
    _ctx->destroy_semaphore(_vk_semaphore);
}

VkSemaphore Semaphore::vk_handle(void) const
{
    return _vk_semaphore;
}

VkPipelineStageFlags Semaphore::get_wait_stages(void) const
{
    return _wait_stages;
}
