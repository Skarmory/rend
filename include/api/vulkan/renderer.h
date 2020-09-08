#ifndef RENDERER_H
#define RENDERER_H

#include <array>
#include <queue>
#include <vector>
#include <vulkan.h>

#include "rend_defs.h"
#include "resource.h"

namespace rend
{

class CommandPool;
class CommandBuffer;
class Fence;
class Framebuffer;
class DepthBuffer;
class RenderPass;
class Semaphore;
class Swapchain;
class UniformBuffer;
class VulkanInstance;
class Window;

class GPUBufferBase;
class GPUTextureBase;
class IndexBuffer;
class VertexBuffer;
class SampledTexture;

struct FrameResources
{
    std::vector<UniformBuffer*> staging_buffers;
    uint32_t                    swapchain_idx{ 0xdeadbeef };
    CommandBuffer*              command_buffer{ nullptr };
    Semaphore*                  acquire_sem{ nullptr };
    Semaphore*                  present_sem{ nullptr };
    Fence*                      submit_fen{ nullptr };
    Framebuffer*                framebuffer{ nullptr };
};

struct Task
{
    virtual ~Task(void) = default;
    virtual void execute(FrameResources& resources) = 0;
};

struct BufferLoadTask : public Task
{
    GPUBufferBase* buffer{ nullptr };
    BufferUsage    buffer_usage{ 0 };
    void*          data{ nullptr };
    size_t         size_bytes{ 0 };
    uint32_t       offset{ 0 };

    BufferLoadTask(GPUBufferBase* buffer, BufferUsage usage, void* data, size_t bytes, uint32_t offset)
        : buffer(buffer), buffer_usage(usage), data(data), size_bytes(bytes), offset(offset) {}
    void execute(FrameResources& resources) override;
};

struct ImageLoadTask : public Task
{
    GPUTextureBase* image{ nullptr };
    ImageUsage      image_usage{ 0 };
    void*           data{ nullptr };
    size_t          size_bytes{ 0 };
    uint32_t        offset{ 0 };

    ImageLoadTask(GPUTextureBase* texture, ImageUsage usage, void* data, size_t bytes, uint32_t offset)
        : image(texture), image_usage(usage), data(data), size_bytes(bytes), offset(offset) {}
    void execute(FrameResources& resources) override;
};

struct ImageTransitionTask : public Task
{
    SampledTexture*      image;
    VkPipelineStageFlags src;
    VkPipelineStageFlags dst;
    VkImageLayout        final_layout;

    ImageTransitionTask(SampledTexture* image, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkImageLayout layout) : image(image), src(src), dst(dst), final_layout(layout) {}
    void execute(FrameResources& resources) override;
};

class Renderer : public rend::Resource
{
public:
    Renderer(const Renderer&)            = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&)      = delete;

    static Renderer& instance(void);
    StatusCode create(const VkPhysicalDeviceFeatures& desired_features, const VkQueueFlags desired_queues);
    StatusCode destroy(void);

    Swapchain*  get_swapchain(void) const;
    RenderPass* get_default_render_pass(void) const;

    // Resource functions
    void load(GPUTextureBase* texture, ImageUsage usage, void* data, size_t bytes, uint32_t offset);
    void load(GPUBufferBase* buffer, BufferUsage usage, void* data, size_t bytes, uint32_t offset);
    void transition(SampledTexture* texture, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkImageLayout final_layout);

    // Functions
    FrameResources& start_frame(void);
    void end_frame(FrameResources& frame_res);
    void resize_resources(void);

private:
    Renderer(void) = default;
    ~Renderer(void);

    // Tasking
    void _process_task_queue(FrameResources& resources);

    void _create_default_depth_buffer(VkExtent2D extent);
    void _create_default_renderpass(void);
    void _create_default_framebuffers(bool recreate);

private:
    Swapchain*                _swapchain { nullptr };
    CommandPool*              _command_pool { nullptr };
    std::vector<Framebuffer*> _default_framebuffers;
    DepthBuffer*              _default_depth_buffer { nullptr };
    RenderPass*               _default_render_pass { nullptr };
    std::queue<Task*>         _task_queue;

    uint32_t                                      _frame_counter { 0 };
    static const uint32_t                         _FRAMES_IN_FLIGHT { 2 };
    std::array<FrameResources, _FRAMES_IN_FLIGHT> _frame_resources;
};

}

#endif
