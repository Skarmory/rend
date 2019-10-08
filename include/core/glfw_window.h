#ifdef GLFW_WINDOW
#ifndef REND_GLFW_WINDOW_H
#define REND_GLFW_WINDOW_H

#include "rend_defs.h"

#ifdef USE_VULKAN
#include "vulkan_window.h"
#endif

struct GLFWwindow;

namespace rend
{

class VulkanInstance;

#ifdef USE_VULKAN
class GLFWWindow : public VulkanWindow
#endif
{
public:
#ifdef USE_VULKAN
    explicit GLFWWindow(VulkanInstance& instance);
#endif
    ~GLFWWindow(void);

    GLFWWindow(const GLFWWindow&)            = delete;
    GLFWWindow(GLFWWindow&&)                 = delete;
    GLFWWindow& operator=(const GLFWWindow&) = delete;
    GLFWWindow& operator=(GLFWWindow&&)      = delete;

    StatusCode create_window_api(uint32_t width, uint32_t height, const char* title);

    GLFWwindow* get_glfw_handle(void) const;

private:
    GLFWwindow* _glfw_window;
};

}

#endif
#endif
