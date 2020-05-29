#include "glfw_window.h"

#include "vulkan_instance.h"

#include <GLFW/glfw3.h>

using namespace rend;
using namespace rend::vkal;

GLFWWindow::~GLFWWindow(void)
{
#ifdef USE_VULKAN
    VulkanInstance::instance().destroy_surface(_vk_surface);
#endif
    glfwDestroyWindow(_glfw_window);
}

StatusCode GLFWWindow::create_window_api(uint32_t width, uint32_t height, const char* title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!_glfw_window)
        return StatusCode::FAILURE;

#ifdef USE_VULKAN
    if(glfwCreateWindowSurface(VulkanInstance::instance().get_handle(), _glfw_window, nullptr, &_vk_surface) != VK_SUCCESS)
        return StatusCode::FAILURE;
#endif

    return StatusCode::SUCCESS;
}

#ifdef USE_VULKAN
VkSurfaceKHR GLFWWindow::get_vk_surface(void) const
{
    return _vk_surface;
}
#endif

GLFWwindow* GLFWWindow::get_glfw_handle(void) const
{
    return _glfw_window;
}
