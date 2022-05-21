#ifndef REND_WINDOW_H
#define REND_WINDOW_H

#include "core/window_base.h"

#include <cstdint>

class GLFWwindow;

namespace rend
{

class Window : public WindowBase
{
public:
    Window(uint32_t width, uint32_t height, const char* title);
    ~Window(void);
    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;

    GLFWwindow* get_handle(void) const;

private:
    GLFWwindow*  _glfw_window{ nullptr };
};

}

#endif
