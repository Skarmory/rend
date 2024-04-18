#include "api/vulkan/fence.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"

using namespace rend;

Fence::Fence(bool start_signalled, VulkanDeviceContext& ctx)
    :
        _ctx(&ctx)
{
    VkFenceCreateInfo create_info = vulkan_helpers::gen_fence_create_info();
    create_info.flags = static_cast<VkFenceCreateFlags>(start_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);
    _vk_fence = _ctx->create_fence(create_info);
}

Fence::~Fence(void)
{
    _ctx->destroy_fence(_vk_fence);
}

VkFence Fence::vk_handle(void) const
{
    return _vk_fence;
}

void Fence::reset(void) const
{
    std::vector<VkFence> fences = { _vk_fence };
    _ctx->get_device()->reset_fences(fences);
}

VkResult Fence::wait(uint64_t timeout) const
{
    std::vector<VkFence> fences = { _vk_fence };
    return _ctx->get_device()->wait_for_fences(fences, timeout, false);
}
