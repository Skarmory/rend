#ifndef REND_API_VULKAN_LAYERS_H
#define REND_API_VULKAN_LAYERS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#include <string>

namespace rend::vk::layer
{
    namespace khronos
    {
        static const char* validation{ "VK_LAYER_KHRONOS_validation" };
    }

    namespace lunarg
    {
        static const char* api_dump{ "VK_LAYER_LUNARG_api_dump" };
        static const char* monitor{ "VK_LAYER_LUNARG_monitor" };
        static const char* screenshot{ "VK_LAYER_LUNARG_screenshot" };
    }

    namespace renderdoc
    {
        static const char* Capture{ "VK_LAYER_RENDERDOC_Capture" };
    }
}

#pragma GCC diagnostic pop

#endif
