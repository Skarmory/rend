#ifndef REND_API_VULKAN_VULKAN_INSTANCE_H
#define REND_API_VULKAN_VULKAN_INSTANCE_H

#include "core/rend_defs.h"

#include <vector>
#include <vulkan.h>

namespace rend
{

class Window;

class VulkanInstance
{
public:
    VulkanInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
    ~VulkanInstance(void);
    VulkanInstance(const VulkanInstance&)           = delete;
    VulkanInstance(VulkanInstance&&)                = delete;
    VulkanInstance operator=(const VulkanInstance&) = delete;
    VulkanInstance operator=(VulkanInstance&&)      = delete;

    void enumerate_physical_devices(std::vector<VkPhysicalDevice>& devices);
    void create_surface(const Window& window);
    VkDebugUtilsMessengerEXT create_debug_utils_messenger(const VkDebugUtilsMessengerCreateInfoEXT& create_info);
    void destroy_debug_utils_messenger(VkDebugUtilsMessengerEXT messenger);

    VkInstance get_handle(void) const;
    VkSurfaceKHR surface(void) const;

private:
    VkInstance   _vk_instance{ VK_NULL_HANDLE };
    VkSurfaceKHR _vk_surface{ VK_NULL_HANDLE };
};

}

#endif
