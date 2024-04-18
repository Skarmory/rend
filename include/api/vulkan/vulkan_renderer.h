#ifndef REND_API_VULKAN_VULKAN_RENDERER_H
#define REND_API_VULKAN_VULKAN_RENDERER_H

#include "api/vulkan/vulkan_buffer.h"
#include "api/vulkan/vulkan_descriptor_set.h"
#include "api/vulkan/vulkan_descriptor_set_layout.h"
#include "api/vulkan/vulkan_framebuffer.h"
#include "api/vulkan/vulkan_pipeline.h"
#include "api/vulkan/vulkan_pipeline_layout.h"
#include "api/vulkan/vulkan_render_pass.h"
#include "api/vulkan/vulkan_shader.h"
#include "api/vulkan/vulkan_texture.h"
#include "core/renderer.h"
#include "core/containers/data_pool.h"
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
    friend class Renderer;

public:
    VulkanRenderer(const Renderer&)            = delete;
    VulkanRenderer(Renderer&&)                 = delete;
    VulkanRenderer& operator=(const Renderer&) = delete;
    VulkanRenderer& operator=(Renderer&&)      = delete;

    void resize(void);

    void configure(void) override;
    void start_frame(void) override;
    void end_frame(void) override;
    void get_size_by_ratio(SizeRatio ratio, uint32_t& width, uint32_t& height) override;

    // Resource functions
    void load_buffer(GPUBuffer& buffer, const void* data, size_t bytes, uint32_t offset) override;
    void load_texture(GPUTexture& texture, const void* data, size_t bytes, uint32_t offset) override;
    void transition(GPUTexture& texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout);
    void write_descriptor_bindings(DescriptorSet* descriptor_set);

    Swapchain* get_swapchain(void) const;
    VulkanDeviceContext* device_context(void) const;

    // Creational
    [[nodiscard]] GPUBuffer*           create_buffer(const std::string& name, const BufferInfo& info) override;
    [[nodiscard]] DescriptorSet*       create_descriptor_set(const std::string& name, const DescriptorSetInfo& info) override;
    [[nodiscard]] DescriptorSetLayout* create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info) override;
    [[nodiscard]] Framebuffer*         create_framebuffer(const std::string& name, const FramebufferInfo& info) override;
    [[nodiscard]] Material*            create_material(const std::string& name, const MaterialInfo& info) override;
    [[nodiscard]] Mesh*                create_mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer) override;
    [[nodiscard]] Pipeline*            create_pipeline(const std::string& name, const PipelineInfo& info) override; 
    [[nodiscard]] PipelineLayout*      create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info) override;
    [[nodiscard]] RenderPass*          create_render_pass(const std::string& name, const RenderPassInfo& info) override;
    [[nodiscard]] Shader*              create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type) override;
    [[nodiscard]] ShaderSet*           create_shader_set(const std::string& name, const ShaderSetInfo& info) override;
    [[nodiscard]] SubPass*             create_sub_pass(const std::string& name, const SubPassInfo& info) override;
    [[nodiscard]] GPUTexture*          create_texture(const std::string& name, const TextureInfo& info) override;

    void destroy_buffer(GPUBuffer* buffer) override;
    void destroy_descriptor_set(DescriptorSet* set) override;
    void destroy_descriptor_set_layout(DescriptorSetLayout* layout) override;
    void destroy_framebuffer(Framebuffer* framebuffer) override;
    void destroy_material(Material* material) override;
    void destroy_mesh(Mesh* mesh) override;
    void destroy_pipeline(Pipeline* pipeline) override;
    void destroy_pipeline_layout(PipelineLayout* pipeline_layout) override;
    void destroy_render_pass(RenderPass* render_pass) override;
    void destroy_shader(Shader* shader) override;
    void destroy_shader_set(ShaderSet* shader_set) override;
    void destroy_sub_pass(SubPass* sub_pass) override;
    void destroy_texture(GPUTexture* texture) override;

private:
    VulkanRenderer(const RendInitInfo& init_info);
    ~VulkanRenderer(void);

    [[nodiscard]] VulkanBuffer* _create_staging_buffer(void);
    [[nodiscard]] GPUTexture*   _register_swapchain_image(const std::string& name, const TextureInfo& info, VkImage image);
    void                        _unregister_swapchain_image(GPUTexture* texture);

    void        _create_descriptor_set_layouts(void);
    void        _setup_frame_datas(void);
    Shader*     _load_shader(const std::string& name, const std::string& path, ShaderStage stage);
    ShaderSet*  _load_shader_set(const Shader& vertex_shader, const Shader& fragment_shader);
    GPUTexture* _create_depth_buffer(VkExtent2D extent);
    void        _create_framebuffers(void);
    RenderPass* _build_forward_render_pass(const ShaderSet& shader_set);
    void        _setup_forward(void);

    void _process_pre_render_tasks(void);
    void _update_uniform_buffers(void);
    std::unordered_map<RenderPass*, std::vector<DrawItem*>> _sort_draw_items(void);
    void _process_draw_items(void);

private:
    VulkanDeviceContext*      _device_context{ nullptr };
    Swapchain*                _swapchain{ nullptr };
    VkCommandPool             _command_pool{ VK_NULL_HANDLE };
    VkDescriptorPool          _descriptor_pool{ VK_NULL_HANDLE };
    DataPool<VulkanBuffer, 8> _staging_buffers;

    DataPool<VulkanBuffer, 64>              _buffers;
    DataPool<VulkanDescriptorSet, 32>       _descriptor_sets;
    DataPool<VulkanDescriptorSetLayout, 32> _descriptor_set_layouts;
    DataPool<VulkanFramebuffer, 32>         _framebuffers;
    DataPool<VulkanPipeline, 32>            _pipelines;
    DataPool<VulkanPipelineLayout, 32>      _pipeline_layouts;
    DataPool<VulkanRenderPass, 32>          _render_passes;
    DataPool<VulkanShader, 32>              _shaders;
    DataPool<VulkanTexture, 32>             _textures;
};

}

#endif
