#ifndef REND_VULKAN_INSTANCE_H
#define REND_VULKAN_INSTANCE_H

#include "rend_defs.h"

#include <vector>
#include <vulkan.h>

namespace rend
{

class VulkanInstance
{
public:
    VulkanInstance(const VulkanInstance&)           = delete;
    VulkanInstance(VulkanInstance&&)                = delete;
    VulkanInstance operator=(const VulkanInstance&) = delete;
    VulkanInstance operator=(VulkanInstance&&)      = delete;

    static VulkanInstance& instance(void);

    StatusCode create_instance(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count);
    void       destroy_instance(void);

    void enumerate_physical_devices(std::vector<VkPhysicalDevice>& devices);
    void destroy_surface(VkSurfaceKHR surface);

    VkInstance get_handle(void) const;

private:
    VulkanInstance(void) = default;
    ~VulkanInstance(void) = default;

    VkInstance _vk_instance{ VK_NULL_HANDLE };
};

}

#endif
