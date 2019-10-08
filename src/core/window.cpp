#include "window.h"

#include <cstdio>
#include <cstring>

using namespace rend;

#if USE_VULKAN
Window::Window(VulkanInstance& instance)
    :
#ifdef GLFW_WINDOW
    GLFWWindow(instance)
#endif
#endif
{
}

Window::~Window(void)
{
}

bool Window::create_window(uint32_t width, uint32_t height, const char* title)
{
    if(create_window_api(width, height, title) != StatusCode::SUCCESS)
    {
        return false;
    }

    _width = width;
    _height = height;
    snprintf(_title, strlen(title), title);

    return true;
}
