#include "core/rend.h"

#include "core/device_context.h"
#include "core/rend_service.h"
#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_device_context.h"

#include <iostream>
#include <vector>

void rend::init_rend(const RendInfo& info)
{
    DeviceContext* context{ nullptr };

    switch(info.api)
    {
        case API::API_VULKAN:
        {
            const VulkanInfo* vulkan_info = static_cast<const VulkanInfo*>(info.api_info);

            VulkanInstance* vulkan_instance = new VulkanInstance(vulkan_info->extensions, vulkan_info->extensions_count, vulkan_info->layers, vulkan_info->layers_count);
            vulkan_instance->create_surface(*RendService::window());
            RendService::provide(vulkan_instance);

            context = new VulkanDeviceContext(vulkan_info->features, vulkan_info->queues);

        }
    }

    RendService::provide(context);
}

void rend::destroy_rend(void)
{
    delete RendService::device_context();
    delete RendService::vulkan_instance();
}
