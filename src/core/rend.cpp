#include "core/rend.h"

#include "core/device_context.h"
#include "core/rend_service.h"
#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_device_context.h"

#include <iostream>
#include <vector>

void rend::init_rend(const RendInitInfo& init_info)
{
    switch(init_info.api)
    {
        case API::API_VULKAN:
        {
            DeviceContext* context = new VulkanDeviceContext(init_info);
            RendService::provide(context);
            break;
        }
    }
}

void rend::destroy_rend(void)
{
    delete RendService::device_context();
}
