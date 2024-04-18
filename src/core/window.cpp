#include "core/window.h"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <stdexcept>
#include <string>

using namespace rend;

Window::Window(uint32_t width, uint32_t height, const char* title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (_glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr); !_glfw_window)
    {
        std::string error_string = "Failed to create GLFW window";
        throw std::runtime_error(error_string);
    }

    resize(width, height);
    set_title(_title);
}

Window::~Window(void)
{
    glfwDestroyWindow(_glfw_window);
}

GLFWwindow* Window::get_handle(void) const
{
    return _glfw_window;
}

bool Window::should_close(void) const
{
    return glfwWindowShouldClose(_glfw_window);
}

void Window::resize(uint32_t width, uint32_t height)
{
    _width  = width;
    _height = height;
}
