#include "core/rend.h"

#include "core/device_context.h"
#include "core/renderer.h"
#include "core/rend_service.h"

#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_device_context.h"
//#include "api/vulkan/vulkan_renderer.h"

//#include <iostream>
//#include <vector>

//void rend::init_rend(const RendInitInfo& init_info)
//{
//    switch(init_info.api)
//    {
//        case API::API_VULKAN:
//        {
//            DeviceContext* context = new VulkanDeviceContext(*static_cast<VulkanInitInfo*>(init_info.api_init_info));
//            RendService::provide(context);
//            Renderer* renderer = new VulkanRenderer(init_info.resource_path);
//            RendService::provide(renderer);
//            break;
//        }
//    }
//}
//
//void rend::destroy_rend(void)
//{
//    delete RendService::renderer();
//    delete RendService::device_context();
//}
