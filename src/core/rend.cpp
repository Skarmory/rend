#include "rend.h"

#include "device_context.h"
#include "vulkan_instance.h"
#include "window.h"
#include "window_context.h"

#include <vector>

void rend::init_rend(Window& window)
{
#ifdef USE_VULKAN
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
		"VK_LAYER_LUNARG_standard_validation"
	};

	// Init singletons
	auto& vk_instance = rend::vkal::VulkanInstance::instance();
	vk_instance.create_instance(extensions.data(), extensions.size(), layers.data(), layers.size());
#endif

	auto& window_context = WindowContext::instance();
	window.create_window();
	window_context.set_window(&window);

	auto& context = DeviceContext::instance();
	context.create();
}

void rend::destroy_rend(void)
{
	DeviceContext::instance().destroy();

	delete WindowContext::instance().window();
	WindowContext::instance().set_window(nullptr);

#ifdef USE_VULKAN
	rend::vkal::VulkanInstance::instance().destroy_instance();
#endif
}
