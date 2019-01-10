#ifndef RENDERER_H
#define RENDERER_H

#include <array>
#include <vector>
#include <vulkan/vulkan.h>

namespace rend
{

class Buffer;
class CommandPool;
class DeviceContext;
class Window;

class Renderer
{
public:
    Renderer(
        Window* window, 
        const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues,
        std::vector<const char*> extensions, std::vector<const char*> layers
    );

    ~Renderer(void);

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&)      = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&)      = delete;

private:
    DeviceContext* _context;
    CommandPool*   _staging_command_pool;

    // Staging buffers
    static const uint32_t _MAX_STAGING_BUFFERS = 4;
    static const uint32_t _STAGING_BUFFER_SIZE = 1024 * 1024 * 4;
    std::array<Buffer*, _MAX_STAGING_BUFFERS> _staging_buffers;
};

}

#endif
