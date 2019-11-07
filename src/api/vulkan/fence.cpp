#include "fence.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

Fence::Fence(DeviceContext& context)
    : _context(context),
      _vk_fence(VK_NULL_HANDLE)
{
}

Fence::~Fence(void)
{
    _context.get_device()->destroy_fence(_vk_fence);
}

bool Fence::create_fence(bool start_signalled)
{
    if(_vk_fence !=  VK_NULL_HANDLE)
        return false;

    VkFenceCreateInfo create_info = vulkan_helpers::gen_fence_create_info();
    create_info.flags = static_cast<VkFenceCreateFlags>(start_signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);

    _vk_fence = _context.get_device()->create_fence(create_info);
    if(_vk_fence == VK_NULL_HANDLE)
        return false;

    return true;
}

VkFence Fence::get_handle(void) const
{
    return _vk_fence;
}

void Fence::reset(void) const
{
    std::vector<VkFence> fences = { _vk_fence };
    _context.get_device()->reset_fences(fences);
}

VkResult Fence::wait(uint64_t timeout) const
{
    std::vector<VkFence> fences = { _vk_fence };
    return _context.get_device()->wait_for_fences(fences, timeout, false);
}
