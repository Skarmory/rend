#ifndef RENDERER_H
#define RENDERER_H

#include <array>
#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan.h>

#include "core/camera.h"
#include "core/containers/data_array.h"
#include "core/containers/data_pool.h"
#include "core/descriptor_pool.h"
#include "core/descriptor_set.h"
#include "core/descriptor_set_layout.h"
#include "core/draw_item.h"
#include "core/framebuffer.h"
#include "core/gpu_buffer.h"
#include "core/gpu_texture.h"
#include "core/material.h"
#include "core/mesh.h"
#include "core/pipeline.h"
#include "core/pipeline_layout.h"
#include "core/rend_defs.h"
#include "core/render_pass.h"
#include "core/shader_set.h"
#include "core/shader.h"
#include "core/sub_pass.h"
#include "core/draw_item.h"

#include "lights/point_light.h"

namespace rend
{

class CommandPool;
class CommandBuffer;
class Fence;
class Semaphore;
class Swapchain;
class Task;

struct FrameResources
{
    uint32_t           swapchain_idx{ 0xdeadbeef };
    CommandBuffer*     command_buffer{ nullptr };
    Semaphore*         acquire_sem{ nullptr };
    Semaphore*         present_sem{ nullptr };
    Fence*             submit_fen{ nullptr };
    FramebufferHandle  framebuffer;
    uint32_t           frame{ 0 };

    std::vector<BufferHandle> staging_buffers_used;
    PerViewData         per_view_data;
};

class Renderer
{
public:
    Renderer(const std::string& resource_path);
    ~Renderer(void);
    Renderer(const Renderer&)            = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&)      = delete;

    void configure(void);

    Swapchain*  get_swapchain(void) const;

    // Resource functions
    void load_texture(TextureHandle texture, const void* data, size_t bytes, uint32_t offset);
    void load_buffer(BufferHandle buffer, const void* data, size_t bytes, uint32_t offset);
    void transition(TextureHandle texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout);

    // Functions
    void start_frame(void);
    void end_frame(void);
    void resize_resources(void);
    void get_size_ratio_dims(SizeRatio size_ratio, uint32_t& width, uint32_t& height);

    [[nodiscard]] BufferHandle              create_buffer(const std::string& name, const BufferInfo& info);
    [[nodiscard]] DescriptorSetLayoutHandle create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info);
    [[nodiscard]] DescriptorSetHandle       create_descriptor_set(DescriptorSetLayoutHandle layout_handle, uint32_t set_number);
    [[nodiscard]] FramebufferHandle         create_framebuffer(const std::string& name, const FramebufferInfo& info);
    [[nodiscard]] PipelineHandle            create_pipeline(const std::string& name, const PipelineInfo& info);
    [[nodiscard]] PipelineHandle            create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info);
    [[nodiscard]] ShaderHandle              create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type);
    [[nodiscard]] ShaderSetHandle           create_shader_set(const std::string& name, const ShaderSetInfo& info);
    [[nodiscard]] SubPassHandle             create_sub_pass(const std::string& name, ShaderSetHandle ss_h, RenderPassHandle rp_h, uint32_t index);
    [[nodiscard]] TextureHandle             create_texture(const std::string& name, const TextureInfo& info);
    [[nodiscard]] RenderPassHandle          create_render_pass(const std::string& name, const RenderPassInfo& info);
    [[nodiscard]] MaterialHandle            create_material(const std::string& name, const MaterialInfo& info);
    [[nodiscard]] MeshHandle                create_mesh(const std::string& name, BufferHandle vertex_buffer, BufferHandle index_buffer);

    void destroy_pipeline(PipelineHandle handle);

    [[nodiscard]] GPUBuffer*      get_buffer(BufferHandle handle) const;
    [[nodiscard]] ShaderSet*      get_shader_set(ShaderSetHandle handle) const;
    [[nodiscard]] GPUBuffer*      get_staging_buffer(void);
    [[nodiscard]] Pipeline*       get_pipeline(PipelineHandle handle) const;
    [[nodiscard]] PipelineLayout* get_pipeline_layout(PipelineLayoutHandle handle) const;
    [[nodiscard]] Material*       get_material(MaterialHandle handle) const;
    [[nodiscard]] Mesh*           get_mesh(MeshHandle handle) const;
    [[nodiscard]] Framebuffer*    get_framebuffer(FramebufferHandle handle) const;
    [[nodiscard]] GPUTexture*     get_texture(TextureHandle handle) const;
    [[nodiscard]] RenderPass*     get_render_pass(RenderPassHandle handle) const;
    [[nodiscard]] DescriptorSet*  get_descriptor_set(DescriptorSetHandle handle) const;
    [[nodiscard]] SubPass*        get_sub_pass(SubPassHandle handle) const;

    [[nodiscard]] TextureHandle    find_texture(const std::string& name) const;
    [[nodiscard]] ShaderHandle     find_shader(const std::string& name) const;
    [[nodiscard]] ShaderSetHandle  find_shader_set(const std::string& name) const;
    [[nodiscard]] RenderPassHandle find_render_pass(const std::string& name) const;

    void add_pre_render_task(std::function<void(void)> func);
    void add_draw_item(const DrawItem& item);
    void add_point_light(glm::vec3 position, const PointLight& light);
    void set_camera(const CameraData& camera);

private:
    void _process_task_queue(FrameResources& resources);
    void _update_uniform_buffers(void);
    std::unordered_map<RenderPassHandle, std::vector<DrawItem*>> _sort_draw_items(void);
    void _process_draw_items(void);

    DescriptorSetHandle _create_descriptor_set_for_material(MaterialHandle handle);

    void             _create_swapchain_textures(void);
    void             _create_descriptor_set_layouts(void);
    void             _setup_frame_resources(void);
    ShaderHandle     _load_shader(const std::string& name, const std::string& path, ShaderStage stage);
    ShaderSetHandle  _load_shader_set(ShaderHandle vertex_shader, ShaderHandle fragment_shader);
    BufferHandle     _create_depth_buffer(VkExtent2D extent);
    void             _create_framebuffers(void);
    RenderPassHandle _build_forward_render_pass(ShaderSetHandle shader_set);
    void             _setup_forward(void);

private:
    Swapchain*                     _swapchain{ nullptr };
    DescriptorPool*                _descriptor_pool{ nullptr };
    CommandPool*                   _command_pool{ nullptr };
    RenderPassHandle               _forward_render_pass{ NULL_HANDLE };
    DescriptorSetLayoutHandle      _per_view_descriptor_set_layout_h{ NULL_HANDLE };
    DescriptorSetLayoutHandle      _per_material_descriptor_set_layout_h{ NULL_HANDLE };
    std::vector<FramebufferHandle> _forward_framebuffers;
    std::vector<TextureHandle>     _swapchain_targets;
    std::queue<std::function<void(void)>> _pre_render_queue;

    uint32_t                                      _frame_counter { 0 };
    uint32_t                                      _current_frame_resources{ 0 };
    static const uint32_t                         _FRAMES_IN_FLIGHT { 2 };
    std::array<FrameResources, _FRAMES_IN_FLIGHT> _frame_resources;

    DataArray<DescriptorSetLayout> _desc_set_layouts;
    DataArray<Framebuffer>         _framebuffers;
    DataArray<GPUBuffer>           _gpu_buffers;
    DataArray<GPUTexture>          _gpu_textures;
    DataArray<Pipeline>            _pipelines;
    DataArray<PipelineLayout>      _pipeline_layouts;
    DataArray<Shader>              _shaders;
    DataArray<ShaderSet>           _shader_sets;
    DataArray<SubPass>             _subpasses;
    DataArray<RenderPass>          _render_passes;
    DataArray<Material>            _materials;
    DataArray<Mesh>                _meshes;

    DataPool<GPUBuffer, 8>         _staging_buffers;

    CameraData                     _camera_data;
    bool                           _camera_data_dirty{ false };
    std::vector<DrawItem>          _draw_items;
    std::string                    _resource_path;

    struct LightUniformData
    {
        size_t     light_count{ 0 };
        glm::vec4  light_positions[POINT_LIGHT_MAX];
        PointLight light_data[POINT_LIGHT_MAX];
    } _light_uniform_data;

    bool _lights_dirty{ false };
};

}

#endif
