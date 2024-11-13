#ifndef REND_API_VULKAN_VULKAN_SEMAPHORE_H
#define REND_API_VULKAN_VULKAN_SEMAPHORE_H

#include "core/rend_defs.h"
#include <string>
#include <vulkan.h>

namespace rend
{

class VulkanDeviceContext;

class Semaphore
{
public:
    Semaphore(const std::string& name, VulkanDeviceContext& ctx, VkPipelineStageFlags wait_stages);
    ~Semaphore(void);
    Semaphore(const Semaphore&)            = delete;
    Semaphore(Semaphore&&)                 = default;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&)      = default;

    VkSemaphore vk_handle(void) const;
    VkPipelineStageFlags get_wait_stages(void) const;

private:
    VkSemaphore _vk_semaphore { VK_NULL_HANDLE };
    std::string _name;
    VkPipelineStageFlags _wait_stages{};
    VulkanDeviceContext* _ctx{ nullptr };
};

}

#endif
