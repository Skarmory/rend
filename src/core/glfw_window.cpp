#include "glfw_window.h"

#include "vulkan_instance.h"

#include <GLFW/glfw3.h>

using namespace rend;

#ifdef USE_VULKAN
GLFWWindow::GLFWWindow(VulkanInstance& instance)
    : VulkanWindow(instance)
#endif
{
}

GLFWWindow::~GLFWWindow(void)
{
    glfwDestroyWindow(_glfw_window);
}

StatusCode GLFWWindow::create_window_api(uint32_t width, uint32_t height, const char* title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!_glfw_window)
        return StatusCode::FAILURE;

#ifdef USE_VULKAN
    if(glfwCreateWindowSurface(_instance.get_handle(), _glfw_window, nullptr, &_vk_surface) != VK_SUCCESS)
        return StatusCode::FAILURE;
#endif

    return StatusCode::SUCCESS;
}

GLFWwindow* GLFWWindow::get_glfw_handle(void) const
{
    return _glfw_window;
}
