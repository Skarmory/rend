#ifndef REND_API_VULKAN_VULKAN_RENDERER_H
#define REND_API_VULKAN_VULKAN_RENDERER_H

#include <string>
#include <unordered_map>
#include <vulkan.h>

#include "core/renderer.h"

#include "core/containers/data_pool.h"

namespace rend
{

class Swapchain;
class CommandPool;

class VulkanRenderer : public Renderer
{
public:
    VulkanRenderer(const std::string& resource_path);
    ~VulkanRenderer(void);
    VulkanRenderer(const Renderer&)            = delete;
    VulkanRenderer(Renderer&&)                 = delete;
    VulkanRenderer& operator=(const Renderer&) = delete;
    VulkanRenderer& operator=(Renderer&&)      = delete;

    void configure(void) override;
    void start_frame(void) override;
    void end_frame(void) override;
    void resize(void) override;
    void get_size_by_ratio(SizeRatio ratio, uint32_t& width, uint32_t& height) override;

    // Resource functions
    void load_texture(TextureHandle texture, const void* data, size_t bytes, uint32_t offset) override;
    void load_buffer(BufferHandle buffer, const void* data, size_t bytes, uint32_t offset) override;
    void transition(TextureHandle texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout);

    Swapchain* get_swapchain(void) const;

private:
    void             _create_swapchain_textures(void);
    void             _create_descriptor_set_layouts(void);
    void             _setup_frame_datas(void);
    ShaderHandle     _load_shader(const std::string& name, const std::string& path, ShaderStage stage);
    ShaderSetHandle  _load_shader_set(ShaderHandle vertex_shader, ShaderHandle fragment_shader);
    BufferHandle     _create_depth_buffer(VkExtent2D extent);
    void             _create_framebuffers(void);
    RenderPassHandle _build_forward_render_pass(ShaderSetHandle shader_set);
    void             _setup_forward(void);

    void _process_pre_render_tasks(void);
    void _update_uniform_buffers(void);
    std::unordered_map<RenderPassHandle, std::vector<DrawItem*>> _sort_draw_items(void);
    void _process_draw_items(void);

private:
    Swapchain*                 _swapchain{ nullptr };
    CommandPool*               _command_pool{ nullptr };
    DataPool<GPUBuffer, 8>     _staging_buffers;
    std::vector<TextureHandle> _swapchain_targets;
};

}

#endif
