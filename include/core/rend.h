#ifndef REND_REND_H
#define REND_REND_H

#include "api/vulkan/device_features.h"

#include <cstdint>
#include <vector>
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
    std::vector<const char*> extensions;
    std::vector<const char*> layers;
    std::vector<DeviceFeature> features;
    VkQueueFlags queues{};
};

struct RendInitInfo
{
    API         api{ API::API_VULKAN };
    void*       api_init_info{ nullptr };
    const char* app_name{ nullptr };
    uint32_t    resolution_width{ 800 };
    uint32_t    resolution_height{ 600 };
};

void rend_initialise(const RendInitInfo& init_info);
void rend_uninitialise(void);

}

#endif
