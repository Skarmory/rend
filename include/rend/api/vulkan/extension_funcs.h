#ifndef REND_API_VULKAN_EXTENSION_FUNCS_H
#define REND_API_VULKAN_EXTENSION_FUNCS_H

#include <vulkan/vulkan.h>

namespace rend
{
    struct InstanceExtensionFuncs
    {
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT{ nullptr };
        PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessengerEXT{ nullptr };
    };

    struct DeviceExtensionFuncs
    {
        PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT{ nullptr };
    };
}

#endif
