#include "fence.h"

#include "device_context.h"
#include "logical_device.h"

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

    _vk_fence = _context.get_device()->create_fence(start_signalled);
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
