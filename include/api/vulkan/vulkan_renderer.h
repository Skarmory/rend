#ifndef REND_API_VULKAN_VULKAN_RENDERER_H
#define REND_API_VULKAN_VULKAN_RENDERER_H

#include "api/vulkan/vulkan_renderer.fdecl.h"

#include "api/vulkan/vulkan_buffer.h"
#include "api/vulkan/vulkan_descriptor_set.h"
#include "api/vulkan/vulkan_descriptor_set_layout.h"
#include "api/vulkan/vulkan_framebuffer.h"
#include "api/vulkan/vulkan_pipeline.h"
#include "api/vulkan/vulkan_pipeline_layout.h"
#include "api/vulkan/vulkan_render_pass.h"
#include "api/vulkan/vulkan_shader.h"
#include "api/vulkan/vulkan_texture.h"
#include "core/containers/data_pool.h"
#include "core/renderer.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan.h>

namespace rend
{

class Swapchain;
class CommandPool;
class VulkanDeviceContext;
class Window;

class VulkanRenderer : public Renderer
{
public:
    VulkanRenderer(const RendInitInfo& init_info);
    ~VulkanRenderer(void);
    VulkanRenderer(const VulkanRenderer&)            = delete;
    VulkanRenderer(VulkanRenderer&&)                 = delete;
    VulkanRenderer& operator=(const VulkanRenderer&) = delete;
    VulkanRenderer& operator=(VulkanRenderer&&)      = delete;

    void resize(void);

    void configure(void) override;
    void start_frame(void) override;
    void end_frame(void) override;
    void get_size_by_ratio(SizeRatio ratio, uint32_t& width, uint32_t& height) override;

    // Resource functions
    void load_buffer(GPUBuffer& buffer) override;
    void load_texture(GPUTexture& texture) override;
    void transition(GPUTexture& texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout);
    void write_descriptor_bindings(const DescriptorSet& descriptor_set);
    void submit_command_buffer(CommandBuffer* command_buffer);

    [[nodiscard]] GPUBuffer*           get_buffer(const std::string& name) const override;
    [[nodiscard]] DescriptorSetLayout* get_descriptor_set_layout(const std::string& name) const override;
    [[nodiscard]] Pipeline*            get_pipeline(const std::string& name) const override;
    [[nodiscard]] Swapchain*           get_swapchain(void) const;
    [[nodiscard]] RenderPass*          get_render_pass(const std::string& name) const override;
    [[nodiscard]] RenderStrategy*      get_render_strategy(const std::string& name) const override;
    [[nodiscard]] GPUTexture*          get_texture(const std::string& name) const override;
    [[nodiscard]] VulkanDeviceContext* device_context(void) const;

    // Creational
    [[nodiscard]] GPUBuffer*           create_buffer(const std::string& name, const BufferInfo& info) override;
    [[nodiscard]] DescriptorSet*       create_descriptor_set(const std::string& name, const DescriptorSetLayout& layout) override;
    [[nodiscard]] DescriptorSetLayout* create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info) override;
                  void                 create_framebuffer(const std::string& name, const FramebufferInfo& info) override;
    [[nodiscard]] Pipeline*            create_pipeline(const std::string& name, const PipelineInfo& info) override; 
    [[nodiscard]] PipelineLayout*      create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info) override;
    [[nodiscard]] RenderPass*          create_render_pass(const std::string& name, const RenderPassInfo& info) override;
                  void                 create_render_target(const std::string& name, const TextureInfo& info) override;
    [[nodiscard]] Shader*              create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type) override;
    [[nodiscard]] GPUTexture*          create_texture(const std::string& name, const TextureInfo& info) override;

    void destroy_buffer(GPUBuffer* buffer) override;
    void destroy_descriptor_set(DescriptorSet* set) override;
    void destroy_descriptor_set_layout(DescriptorSetLayout* layout) override;
    void destroy_framebuffer(Framebuffer* framebuffer) override;
    void destroy_pipeline(Pipeline* pipeline) override;
    void destroy_pipeline_layout(PipelineLayout* pipeline_layout) override;
    void destroy_render_pass(RenderPass* render_pass) override;
    void destroy_shader(Shader* shader) override;
    void destroy_texture(GPUTexture* texture) override;

protected:
    void _resize(void) override;

private:
    [[nodiscard]] Framebuffer*   _create_framebuffer(const std::string& name, const FramebufferInfo& info);
    [[nodiscard]] PerPassData    _create_per_pass_data(const Framebuffer& fb, const ColourClear& colour_clear, const DepthStencilClear& depth_clear, const RenderArea& render_area);

    VulkanBuffer* _create_staging_buffer(void);
    void _destroy_staging_buffer(VulkanBuffer* buffer);

    void _process_pre_render_tasks(void);
    std::unordered_map<View*, std::unordered_map<RenderStrategy*, std::vector<DrawItem*>>> _sort_draw_items(void);
    void _process_draw_items(void);

private: // vars
    static const int _STAGING_BUFFERS_MAX = 8;

    VulkanDeviceContext*      _device_context{ nullptr };
    Swapchain*                _swapchain{ nullptr };
    VkCommandPool             _command_pool{ VK_NULL_HANDLE };
    VkDescriptorPool          _descriptor_pool{ VK_NULL_HANDLE };

    DataPool<VulkanBuffer, _STAGING_BUFFERS_MAX> _staging_buffers;
    DataArray<VulkanBuffer> _buffers;
    DataArray<VulkanDescriptorSet> _descriptor_sets;
    DataArray<VulkanDescriptorSetLayout> _descriptor_set_layouts;
    DataArray<VulkanFramebuffer> _framebuffers;
    DataArray<VulkanPipeline> _pipelines;
    DataArray<VulkanPipelineLayout> _pipeline_layouts;
    DataArray<VulkanRenderPass> _render_passes;
    DataArray<VulkanShader> _shaders;
    DataArray<VulkanTexture> _textures;
};

}

#endif
