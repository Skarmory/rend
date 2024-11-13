#include "api/vulkan/fence.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "core/logging/log_defs.h"
#include "core/logging/log_manager.h"

using namespace rend;

Fence::Fence(const std::string& name, bool start_signalled, VulkanDeviceContext& ctx)
    :
        _name(name),
        _ctx(&ctx)
{
    VkFenceCreateInfo create_info = vulkan_helpers::gen_fence_create_info();
    create_info.flags = static_cast<VkFenceCreateFlags>(start_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);
    _vk_fence = _ctx->create_fence(create_info);

#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "FENCE | Creating fence (" + name + ") with params: { start signalled: " + std::to_string(start_signalled) + " }");
    _ctx->set_debug_name(name, VK_OBJECT_TYPE_FENCE, (uint64_t)_vk_fence);
#endif
}

Fence::~Fence(void)
{
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "FENCE | Destroying fence (" + _name + ")");
    _ctx->destroy_fence(_vk_fence);
}

VkFence Fence::vk_handle(void) const
{
    return _vk_fence;
}

void Fence::reset(void) const
{
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "FENCE | Resetting fence (" + _name + ")");
    std::vector<VkFence> fences = { _vk_fence };
    _ctx->get_device()->reset_fences(fences);
}

VkResult Fence::wait(uint64_t timeout) const
{
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "FENCE | Waiting on fence (" + _name + ")");
    std::vector<VkFence> fences = { _vk_fence };
    return _ctx->get_device()->wait_for_fences(fences, timeout, false);
}
