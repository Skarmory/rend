#include "window.h"

#include <stdexcept>

using namespace rend;

Window::Window(uint32_t width, uint32_t height, const char* title) : _width(width), _height(height), _title(title)
{
}

Window::~Window(void)
{
    vkDestroySurfaceKHR(_vk_instance, _vk_surface, nullptr); 
}

uint32_t Window::get_width(void) const
{
    return _width;
}

uint32_t Window::get_height(void) const
{
    return _height;
}

const char* Window::get_title(void) const
{
    return _title;
}

VkSurfaceKHR Window::_create_surface_private(VkInstance instance)
{
    _vk_instance = instance;
    _create_surface(_vk_instance, &_vk_surface);
    return _vk_surface;
}

#ifdef GLFW_WINDOW

GLFWWindow::GLFWWindow(uint32_t width, uint32_t height, const char* title) : Window(width, height, title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

GLFWWindow::~GLFWWindow(void)
{
    glfwDestroyWindow(_window);
}

GLFWwindow* GLFWWindow::get_window_handle(void) const
{
    return _window;
}

void GLFWWindow::_create_surface(VkInstance instance, VkSurfaceKHR* surface)
{
    if( glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS )
        throw std::runtime_error("Failed to create window surface");
}

#endif
