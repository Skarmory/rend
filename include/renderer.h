#ifndef RENDERER_H
#define RENDERER_H

#include <array>
#include <queue>
#include <vector>
#include <vulkan/vulkan.h>

#include "rend_defs.h"

namespace rend
{

class Buffer;
class CommandPool;
class CommandBuffer;
class DeviceContext;
class Fence;
class Framebuffer;
class GPUBuffer;
class Image;
class RenderPass;
class Semaphore;
class Swapchain;
class Texture2D;
class Window;

class IndexBuffer;

enum class ResourceType
{
    BUFFER,
    TEXTURE2D,
    INDEX_BUFFER
};

struct FrameResources
{
    std::vector<GPUBuffer*> staging_buffers;
    uint32_t                swapchain_idx;
    CommandBuffer*          command_buffer;
    Semaphore*              acquire_sem;
    Semaphore*              present_sem;
    Fence*                  submit_fen;
};

struct Task
{
    virtual ~Task(void) = default;
    virtual void execute(DeviceContext* context, FrameResources* resources) = 0;
};

struct LoadTask : public Task
{
    void*        resource;
    ResourceType resource_type;
    void*        data;
    size_t       size_bytes;

    LoadTask(void* resource, ResourceType type, void* data, size_t bytes) : resource(resource), resource_type(type), data(data), size_bytes(bytes) {}
    virtual void execute(DeviceContext* context, FrameResources* resources) override;
};

struct ImageTransitionTask : public Task
{
    Texture2D*           image;
    VkPipelineStageFlags src;
    VkPipelineStageFlags dst;
    VkImageLayout        final_layout;

    ImageTransitionTask(Texture2D* image, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkImageLayout layout) : image(image), src(src), dst(dst), final_layout(layout) {}
    virtual void execute(DeviceContext* context, FrameResources* resources) override;
};

class Renderer
{
public:
    Renderer(
        Window* window, 
        const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues,
        std::vector<const char*> extensions, std::vector<const char*> layers
    );

    ~Renderer(void);

    Renderer(const Renderer&)            = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&)      = delete;

    DeviceContext* get_device_context(void) const;
    Swapchain*     get_swapchain(void) const;
    RenderPass*    get_default_render_pass(void) const;

    // Creational
    Buffer*    create_vertex_buffer(size_t vertex_count, size_t vertex_size);
    IndexBuffer* create_index_buffer(uint32_t index_count, size_t index_size);
    Buffer*    create_uniform_buffer(size_t size_bytes);
    Texture2D* create_diffuse(uint32_t width, uint32_t height, uint32_t mip_levels);
    Texture2D* create_depth_buffer(uint32_t width, uint32_t height);

    // Resource functions
    void load(void* resource, ResourceType type, void* data, size_t bytes);
    void transition(Texture2D* texture, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkImageLayout final_layout);

    // Functions
    FrameResources* start_frame(void);
    void end_frame(FrameResources* frame_res);
    void begin_render_pass(FrameResources* frame_res, std::vector<VkClearValue>& clear_values, VkRect2D render_area={ 0, 0, 0, 0 });
    void end_render_pass(FrameResources* frame_res);
    void resize_resources(void);

private:
    // Tasking
    void _process_task_queue(FrameResources* resources);

    void _create_default_renderpass(void);
    void _create_default_framebuffers(bool recreate);

private:
    DeviceContext* _context;
    Swapchain*     _swapchain;
    CommandPool*   _command_pool;

    std::vector<Framebuffer*> _default_framebuffers;
    Image* _default_depth_buffer;
    RenderPass* _default_render_pass;

    std::queue<Task*>       _task_queue;

    uint32_t _frame_counter;
    static const uint32_t _FRAMES_IN_FLIGHT = 2;
    std::array<FrameResources, _FRAMES_IN_FLIGHT> _frame_resources;
};

}

#endif
