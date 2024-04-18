#ifndef REND_REND_H
#define REND_REND_H

#include <cstdint>
#include <vulkan.h>

namespace rend
{

class Window;

enum class API
{
    API_VULKAN
};

struct VulkanInitInfo
{
    const char** extensions{ nullptr };
    uint32_t     extensions_count{ 0 };
    const char** layers{ nullptr };
    uint32_t     layers_count{ 0 };
    VkPhysicalDeviceFeatures features{};
    VkQueueFlags queues{};
};

struct RendInitInfo
{
    API         api{ API::API_VULKAN };
    void*       api_init_info{ nullptr };
    const char* app_name{ nullptr };
    uint32_t    resolution_width{ 800 };
    uint32_t    resolution_height{ 600 };
    const char* resource_path{ nullptr}; //TODO Temporary whilst I figure out how to deal with loading from the dll
};

//void init_rend(const RendInitInfo& info);
//void destroy_rend(void);

}

#endif
