#include "api/vulkan/vulkan_semaphore.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "core/logging/log_defs.h"
#include "core/logging/log_manager.h"

using namespace rend;

Semaphore::Semaphore(const std::string& name, VulkanDeviceContext& ctx, VkPipelineStageFlags wait_stages)
    :
        _name(name),
        _wait_stages(wait_stages),
        _ctx(&ctx)
{
    VkSemaphoreCreateInfo create_info = vulkan_helpers::gen_semaphore_create_info();
    _vk_semaphore = _ctx->create_semaphore(create_info);

#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SEMAPHORE | Creating semaphore (" + _name + ")");
    _ctx->set_debug_name(name, VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)_vk_semaphore);
#endif
}

Semaphore::~Semaphore(void)
{
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SEMAPHORE | Destroying semaphore (" + _name + ")");
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
