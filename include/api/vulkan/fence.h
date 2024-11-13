#ifndef REND_API_VULKAN_FENCE_H
#define REND_API_VULKAN_FENCE_H

#include <limits>
#include <string>
#include <vulkan.h>

namespace rend
{

class VulkanDeviceContext;

class Fence
{
public:
    Fence(const std::string& name, bool start_signalled, VulkanDeviceContext& ctx);
    Fence(void);
    ~Fence(void);
    Fence(const Fence&)            = delete;
    Fence(Fence&&)                 = default;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&)      = default;

    VkFence  vk_handle(void) const;
    void     reset(void) const;
    VkResult wait(uint64_t timeout=std::numeric_limits<uint64_t>::max()) const;

private:
    std::string _name;
    VkFence _vk_fence { VK_NULL_HANDLE };
    VulkanDeviceContext* _ctx{ nullptr };
};

}

#endif
