#include "fence.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

Fence::Fence(LogicalDevice* device, bool start_signalled) : _device(device)
{
    VkFenceCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkFenceCreateFlags>(start_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0)
    };

    VULKAN_DEATH_CHECK(vkCreateFence(_device->get_handle(), &create_info, nullptr, &_vk_fence), "Failed to create fence");
}

Fence::~Fence(void)
{
    vkDestroyFence(_device->get_handle(), _vk_fence, nullptr);
}

VkFence Fence::get_handle(void) const
{
    return _vk_fence;
}

void Fence::reset(void) const
{
    vkResetFences(_device->get_handle(), 1, &_vk_fence);
}

VkResult Fence::wait(uint64_t timeout) const
{
    return vkWaitForFences(_device->get_handle(), 1, &_vk_fence, false, timeout);
}
