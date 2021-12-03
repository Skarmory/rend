#ifdef GLFW_WINDOW
#ifndef REND_GLFW_WINDOW_H
#define REND_GLFW_WINDOW_H

#include "core/rend_defs.h"
#include "core/window_base.h"

#include <vulkan.h>

struct GLFWwindow;

namespace rend
{

class GLFWWindow : public WindowBase
{
public:
    GLFWWindow(void) = default;
    ~GLFWWindow(void);

    GLFWWindow(const GLFWWindow&)            = delete;
    GLFWWindow(GLFWWindow&&)                 = delete;
    GLFWWindow& operator=(const GLFWWindow&) = delete;
    GLFWWindow& operator=(GLFWWindow&&)      = delete;

    StatusCode create_window_api(uint32_t width, uint32_t height, const char* title);

    VkSurfaceKHR get_vk_surface(void) const;

    GLFWwindow* get_glfw_handle(void) const;

private:
    GLFWwindow* _glfw_window{ nullptr };

    VkSurfaceKHR _vk_surface{ VK_NULL_HANDLE };
};

}

#endif
#endif
