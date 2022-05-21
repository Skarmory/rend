#ifndef REND_REND_SERVICE_H
#define REND_REND_SERVICE_H

namespace rend
{

class DeviceContext;
class Renderer;
class Window;
class VulkanInstance;
struct RendInfo;

class RendService
{
    public:
        static void provide(DeviceContext* context);
        static void provide(Renderer* renderer);
        static void provide(VulkanInstance* vulkan_instance);
        static void provide(Window* window);

        static DeviceContext*  device_context(void);
        static Renderer*       renderer(void);
        static Window*         window(void);
        static VulkanInstance* vulkan_instance(void);

    private:
        static DeviceContext*  _context;
        static Renderer*       _renderer;
        static VulkanInstance* _vulkan_instance;
        static Window*         _window;
};

}

#endif

