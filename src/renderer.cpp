#include <renderer.h>

#include <iostream>

using namespace rend;

Renderer::Renderer(Window* window, const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues, std::vector<const char*> extensions, std::vector<const char*> layers)
{
    /*
    _context = new DeviceContext(extensions.data(), extensions.size(), layers.data(), layers.size(), window);

    _context->create_device(desired_features, desired_flags);
    */
}

Renderer::~Renderer(void)
{
}
