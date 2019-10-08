#ifdef USE_VULKAN
#ifndef REND_VULKAN_INSTANCE_H
#define REND_VULKAN_INSTANCE_H

#include "rend_defs.h"

#include <vulkan.h>

namespace rend
{

class VulkanInstance
{
public:
    VulkanInstance(void);
    ~VulkanInstance(void);

    VulkanInstance(const VulkanInstance&)           = delete;
    VulkanInstance(VulkanInstance&&)                = delete;
    VulkanInstance operator=(const VulkanInstance&) = delete;
    VulkanInstance operator=(VulkanInstance&&)      = delete;

    StatusCode create_instance(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count);

    VkInstance get_handle(void) const;

private:
    VkInstance _vk_instance;
};

}

#endif
#endif
