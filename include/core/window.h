#ifndef REND_WINDOW_H
#define REND_WINDOW_H

#include <cstdint>

#ifdef GLFW_WINDOW
#include "core/glfw_window.h"
#endif

namespace rend
{

#ifdef GLFW_WINDOW
class Window : public GLFWWindow
#endif
{
public:
    Window(void)                     = default;
    ~Window(void)                    = default;
    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;

    [[nodiscard]] bool create_window(uint32_t width, uint32_t height, const char* title);
    [[nodiscard]] bool create_window(void);

    void destroy_window(void);
};

}

#endif
