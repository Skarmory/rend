#include "core/rend.h"

#include "api/vulkan/vulkan_instance.h"

#include "core/device_context.h"
#include "api/vulkan/vulkan_device_context.h"
#include "core/window.h"
#include "core/window_context.h"

#include <iostream>
#include <vector>

namespace
{
    void init_vulkan(const rend::VulkanInfo& info)
    {
        auto& vk_instance = rend::VulkanInstance::instance();
        if(vk_instance.create(info.extensions, info.extensions_count, info.layers, info.layers_count) == rend::StatusCode::FAILURE)
        {
            std::cerr << "Failed to create vulkan instance!" << std::endl;
            std::abort();
        }
    }

    void init_window(const rend::RendInfo& info)
    {
        auto& window_context = rend::WindowContext::instance();
        if(!info.window->create_window())
        {
            std::cerr << "Failed to create window!" << std::endl;
            std::abort();
        }

        window_context.set_window(info.window);
    }

    rend::DeviceContext* ctx{ nullptr };
}

void rend::init_rend(const RendInfo& info)
{
    switch(info.api)
    {
        case API::API_VULKAN:
        {
            ::init_vulkan(*static_cast<const VulkanInfo*>(info.api_info));
            ::init_window(info);
            ::ctx = new rend::VulkanDeviceContext;
        }
    }

    ::ctx->create();
}

void rend::destroy_rend(void)
{
	DeviceContext::instance().destroy();
    delete ctx;

    WindowContext::instance().window()->destroy_window();
	WindowContext::instance().set_window(nullptr);

	VulkanInstance::instance().destroy();
}
