#ifdef GLFW_WINDOW
#ifndef REND_GLFW_WINDOW_H
#define REND_GLFW_WINDOW_H

#include "rend_defs.h"
#include "window_base.h"

#ifdef USE_VULKAN
#include <vulkan.h>
#endif

struct GLFWwindow;

namespace rend
{

#ifdef USE_VULKAN
class VulkanInstance;
#endif

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

#ifdef USE_VULKAN
    void set_vulkan_instance(VulkanInstance& instance);
    VkSurfaceKHR get_vk_surface(void) const;
#endif

    GLFWwindow* get_glfw_handle(void) const;

private:
    GLFWwindow* _glfw_window { nullptr };

#ifdef USE_VULKAN
    VulkanInstance* _vulkan_instance { nullptr };
    VkSurfaceKHR    _vk_surface      { nullptr };
#endif
};

}

#endif
#endif
