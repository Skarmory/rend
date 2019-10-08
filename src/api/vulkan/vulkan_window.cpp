#include "vulkan_window.h"

#include "vulkan_instance.h"

using namespace rend;

VulkanWindow::VulkanWindow(VulkanInstance& instance)
    : _instance(instance),
      _vk_surface(VK_NULL_HANDLE)
{
}

VulkanWindow::~VulkanWindow(void)
{
    vkDestroySurfaceKHR(_instance.get_handle(), _vk_surface, nullptr);
}

VkSurfaceKHR VulkanWindow::get_handle(void) const
{
    return _vk_surface;
}
