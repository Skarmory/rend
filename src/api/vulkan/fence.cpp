#include "fence.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_device_context.h"

using namespace rend;

bool Fence::create(bool start_signalled)
{
    if(_vk_fence !=  VK_NULL_HANDLE)
    {
        return false;
    }

    VkFenceCreateInfo create_info = vulkan_helpers::gen_fence_create_info();
    create_info.flags = static_cast<VkFenceCreateFlags>(start_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    _vk_fence = ctx.get_device()->create_fence(create_info);
    if(_vk_fence == VK_NULL_HANDLE)
    {
        return false;
    }

    return true;
}

void Fence::destroy(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->destroy_fence(_vk_fence);
}

VkFence Fence::get_handle(void) const
{
    return _vk_fence;
}

void Fence::reset(void) const
{
    std::vector<VkFence> fences = { _vk_fence };
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    ctx.get_device()->reset_fences(fences);
}

VkResult Fence::wait(uint64_t timeout) const
{
    std::vector<VkFence> fences = { _vk_fence };
    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    return ctx.get_device()->wait_for_fences(fences, timeout, false);
}
