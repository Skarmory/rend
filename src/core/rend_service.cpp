#include "core/rend_service.h"

using namespace rend;

// public

void RendService::provide(DeviceContext* context)
{
    _context = context;
}

void RendService::provide(Renderer* renderer)
{
    _renderer = renderer;
}

void RendService::provide(VulkanInstance* vulkan_instance)
{
    _vulkan_instance = vulkan_instance;
}

void RendService::provide(Window* window)
{
    _window = window;
}

DeviceContext* RendService::device_context(void)
{
    return _context;
}

Renderer* RendService::renderer(void)
{
    return _renderer;
}

Window* RendService::window(void)
{
    return _window;
}

VulkanInstance* RendService::vulkan_instance(void)
{
    return _vulkan_instance;
}

DeviceContext* RendService::_context = nullptr;
Renderer* RendService::_renderer = nullptr;
VulkanInstance* RendService::_vulkan_instance = nullptr;
Window* RendService::_window = nullptr;
