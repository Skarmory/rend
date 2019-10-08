#ifndef REND_WINDOW_H
#define REND_WINDOW_H

#include <cstdint>

#ifdef GLFW_WINDOW
#include "glfw_window.h"
#endif

namespace rend
{

class VulkanInstance;

#ifdef GLFW_WINDOW
class Window : public GLFWWindow
#endif
{
public:
#ifdef USE_VULKAN
    explicit Window(VulkanInstance& instance);
#endif
    ~Window(void);

    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;

    bool create_window(uint32_t width, uint32_t height, const char* title);
};

}

#endif
