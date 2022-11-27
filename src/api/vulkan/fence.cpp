#include "api/vulkan/fence.h"

#include "core/device_context.h"
#include "core/rend_service.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_device_context.h"

#include <cassert>

using namespace rend;

Fence::Fence(bool start_signalled)
{
    VkFenceCreateInfo create_info = vulkan_helpers::gen_fence_create_info();
    create_info.flags = static_cast<VkFenceCreateFlags>(start_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);

    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());
    _vk_fence = ctx.create_fence(create_info);
}

Fence::Fence(void)
    :
        Fence(false)
{
}

Fence::~Fence(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());
    ctx.destroy_fence(_vk_fence);
}

VkFence Fence::handle(void) const
{
    return _vk_fence;
}

void Fence::reset(void) const
{
    std::vector<VkFence> fences = { _vk_fence };
    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());
    ctx.get_device()->reset_fences(fences);
}

VkResult Fence::wait(uint64_t timeout) const
{
    std::vector<VkFence> fences = { _vk_fence };
    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());
    return ctx.get_device()->wait_for_fences(fences, timeout, false);
}
