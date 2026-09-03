#ifndef REND_WINDOW_H
#define REND_WINDOW_H

#include "rend/rend_api.h"
#include "rend/core/window_base.h"

#include <cstdint>

class GLFWwindow;

namespace rend
{

class REND_API Window : public WindowBase
{
public:
    Window(uint32_t width, uint32_t height, const char* title);
    ~Window(void);
    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;

    void* get_handle(void) const;
    bool should_close(void) const;
    void resize(uint32_t width, uint32_t height) override;

private:
    void*  _glfw_window{ nullptr };
};

}

#endif
