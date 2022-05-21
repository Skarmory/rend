#ifndef RENDERER_H
#define RENDERER_H

#include <array>
#include <queue>
#include <vector>
#include <vulkan.h>

#include "core/rend_defs.h"
#include "core/resource.h"

namespace rend
{

class CommandPool;
class CommandBuffer;
class Fence;
class Framebuffer;
class RenderPass;
class Semaphore;
class Swapchain;
class UniformBuffer;
class VulkanInstance;
class Window;

class GPUBuffer;
class GPUTexture;
class IndexBuffer;
class VertexBuffer;

struct FrameResources
{
    std::vector<GPUBuffer*> staging_buffers;
    uint32_t                swapchain_idx{ 0xdeadbeef };
    CommandBuffer*          command_buffer{ nullptr };
    Semaphore*              acquire_sem{ nullptr };
    Semaphore*              present_sem{ nullptr };
    Fence*                  submit_fen{ nullptr };
    Framebuffer*            framebuffer{ nullptr };
    uint32_t                frame{ 0 };
};

struct Task
{
    virtual ~Task(void) = default;
    virtual void execute(FrameResources& resources) = 0;
};

struct BufferLoadTask : public Task
{
    GPUBuffer*  buffer{ nullptr };
    BufferUsage buffer_usage{ 0 };
    const void* data{ nullptr };
    size_t      size_bytes{ 0 };
    uint32_t    offset{ 0 };

    BufferLoadTask(GPUBuffer* buffer, BufferUsage usage, const void* data, size_t bytes, uint32_t offset)
        : buffer(buffer), buffer_usage(usage), data(data), size_bytes(bytes), offset(offset) {}
    void execute(FrameResources& resources) override;
};

struct ImageLoadTask : public Task
{
    GPUTexture* image{ nullptr };
    ImageUsage  image_usage{ 0 };
    const void* data{ nullptr };
    size_t      size_bytes{ 0 };
    uint32_t    offset{ 0 };

    ImageLoadTask(GPUTexture* texture, ImageUsage usage, const void* data, size_t bytes, uint32_t offset)
        : image(texture), image_usage(usage), data(data), size_bytes(bytes), offset(offset) {}
    void execute(FrameResources& resources) override;
};

struct ImageTransitionTask : public Task
{
    GPUTexture*    image;
    PipelineStages src;
    PipelineStages dst;
    ImageLayout    final_layout;

    ImageTransitionTask(GPUTexture* image, PipelineStages src, PipelineStages dst, ImageLayout layout) : image(image), src(src), dst(dst), final_layout(layout) {}
    void execute(FrameResources& resources) override;
};

class Renderer : public rend::Resource
{
public:
    Renderer(void);
    ~Renderer(void);
    Renderer(const Renderer&)            = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&)      = delete;

    Swapchain*  get_swapchain(void) const;
    RenderPass* get_default_render_pass(void) const;

    // Resource functions
    void load(GPUTexture* texture, ImageUsage usage, const void* data, size_t bytes, uint32_t offset);
    void load(GPUBuffer* buffer, BufferUsage usage, const void* data, size_t bytes, uint32_t offset);
    void transition(GPUTexture* texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout);

    // Functions
    FrameResources& start_frame(void);
    void end_frame(FrameResources& frame_res);
    void resize_resources(void);

private:
    // Tasking
    void _process_task_queue(FrameResources& resources);

    void _create_default_depth_buffer(VkExtent2D extent);
    void _create_default_renderpass(void);
    void _create_default_framebuffers(bool recreate);

private:
    Swapchain*                _swapchain { nullptr };
    CommandPool*              _command_pool;
    std::vector<Framebuffer*> _default_framebuffers;
    GPUTexture*               _default_depth_buffer { nullptr };
    RenderPass*               _default_render_pass { nullptr };
    std::queue<Task*>         _task_queue;

    uint32_t                                      _frame_counter { 0 };
    static const uint32_t                         _FRAMES_IN_FLIGHT { 2 };
    std::array<FrameResources, _FRAMES_IN_FLIGHT> _frame_resources;
};

}

#endif
