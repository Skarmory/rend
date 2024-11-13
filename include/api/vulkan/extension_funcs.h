#ifndef REND_API_VULKAN_EXTENSION_FUNCS_H
#define REND_API_VULKAN_EXTENSION_FUNCS_H

#include <vulkan.h>

namespace rend
{
    extern "C"
    {
        extern PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT;
        extern PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT;
        extern PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessengerEXT;
    }
}

#endif
