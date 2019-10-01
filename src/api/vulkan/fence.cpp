#include "fence.h"

#include "device_context.h"
#include "logical_device.h"
#include "utils.h"

using namespace rend;

Fence::Fence(DeviceContext* context)
    : _context(context),
      _vk_fence(VK_NULL_HANDLE)
{
}

Fence::~Fence(void)
{
    vkDestroyFence(_context->get_device()->get_handle(), _vk_fence, nullptr);
}

bool Fence::create_fence(bool start_signalled)
{
    if(_vk_fence !=  VK_NULL_HANDLE)
        return false;

    VkFenceCreateInfo create_info =
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkFenceCreateFlags>(start_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0)
    };

    if(vkCreateFence(_context->get_device()->get_handle(), &create_info, nullptr, &_vk_fence) != VK_SUCCESS)
        return false;

    return true;
}

VkFence Fence::get_handle(void) const
{
    return _vk_fence;
}

void Fence::reset(void) const
{
    vkResetFences(_context->get_device()->get_handle(), 1, &_vk_fence);
}

VkResult Fence::wait(uint64_t timeout) const
{
    return vkWaitForFences(_context->get_device()->get_handle(), 1, &_vk_fence, false, timeout);
}
