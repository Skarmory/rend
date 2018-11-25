#ifndef WINDOW_H
#define WINDOW_H

#include <vulkan.h>

namespace rend
{

class Window
{
    friend class DeviceContext;

// Methods
public:
    Window(uint32_t width, uint32_t height, const char* title);
    virtual ~Window(void);

    uint32_t    get_width(void) const;
    uint32_t    get_height(void) const;
    const char* get_title(void) const;

    virtual void resize(uint32_t width, uint32_t height) = 0;

protected:
    virtual void _create_surface(VkInstance instance, VkSurfaceKHR* surface) = 0;

private:
    VkSurfaceKHR _create_surface_private(VkInstance instance);

// Variables
protected:
    uint32_t _width;
    uint32_t _height;
    const char* _title;

private:
    VkInstance   _vk_instance;
    VkSurfaceKHR _vk_surface;
};

}

#ifdef GLFW_WINDOW

#include <GLFW/glfw3.h>

namespace rend
{

class GLFWWindow : public Window
{
public:
    GLFWWindow(uint32_t width, uint32_t height, const char* title);
    ~GLFWWindow(void);

    uint32_t    get_width(void) const;
    uint32_t    get_height(void) const;
    GLFWwindow* get_window_handle(void) const;

    void resize(uint32_t width, uint32_t height);

protected:
    void _create_surface(VkInstance instance, VkSurfaceKHR* surface) override;

private:
    GLFWwindow* _window;
};

}

#endif

#endif
