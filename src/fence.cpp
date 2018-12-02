#include "fence.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

Fence::Fence(LogicalDevice* device) : _device(device)
{
    VkFenceCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
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
