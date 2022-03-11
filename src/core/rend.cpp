#include "core/rend.h"

#include "core/device_context.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_instance.h"
#include "core/window.h"
#include "core/window_context.h"

#include <iostream>
#include <vector>

void rend::init_rend(Window& window)
{
	std::vector<const char*> extensions =
	{
		"VK_KHR_surface"
#ifdef __linux__
		, "VK_KHR_xcb_surface"
#elif defined(_WIN32)
		, "VK_KHR_win32_surface"
#endif
	};

	std::vector<const char*> layers =
	{
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_KHRONOS_validation"
	};

	// Init singletons
	auto& vk_instance = VulkanInstance::instance();
	vk_instance.create(extensions.data(), extensions.size(), layers.data(), layers.size());

	auto& window_context = WindowContext::instance();
	if(!window.create_window())
    {
        std::cerr << "Failed to create window!" << std::endl;
        std::abort();
    }

	window_context.set_window(&window);

	DeviceContext* ctx = new VulkanDeviceContext;
	ctx->create();
}

void rend::destroy_rend(void)
{
	DeviceContext::instance().destroy();

	WindowContext::instance().set_window(nullptr);

	VulkanInstance::instance().destroy();
}
