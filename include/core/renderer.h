#ifndef REND_CORE_RENDERER_H
#define REND_CORE_RENDERER_H

#include <functional>
#include <queue>
#include <string>
#include <vector>

#include "core/descriptor_set_layout.h"
#include "core/camera.h"
#include "core/frame.h"
#include "core/framebuffer.h"
#include "core/gpu_buffer.h"
#include "core/gpu_texture.h"
#include "core/material.h"
#include "core/mesh.h"
#include "core/pipeline.h"
#include "core/pipeline_layout.h"
#include "core/render_pass.h"
#include "core/shader.h"
#include "core/shader_set.h"
#include "core/sub_pass.h"

#include "core/containers/data_array.h"

#include "lights/point_light.h"
#include "lights/light_uniform_data.h"

namespace rend
{

class DescriptorSet;
class DescriptorPool;

class Renderer
{
    public:
        Renderer(const std::string& resource_path);
        virtual ~Renderer(void) = default;

        virtual void configure(void) = 0;
        virtual void start_frame(void) = 0;
        virtual void end_frame(void) = 0;
        virtual void resize(void) = 0;
        virtual void get_size_by_ratio(SizeRatio size_ratio, uint32_t& width, uint32_t& height) = 0;

        virtual void load_texture(TextureHandle texture, const void* data, size_t bytes, uint32_t offset) = 0;
        virtual void load_buffer(BufferHandle buffer, const void* data, size_t bytes, uint32_t offset) = 0;

        void add_draw_item(const DrawItem& draw_item);
        void add_pre_render_task(std::function<void(void)> task_func);
        void add_point_light(glm::vec3 position, const PointLight& light);
        void set_camera(const CameraData& camera);

        [[nodiscard]] BufferHandle              create_buffer(const std::string& name, const BufferInfo& info);
        [[nodiscard]] DescriptorSetHandle       create_descriptor_set(DescriptorSetLayoutHandle layout_handle, uint32_t set_number);
        [[nodiscard]] DescriptorSetLayoutHandle create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info);
        [[nodiscard]] FramebufferHandle         create_framebuffer(const std::string& name, const FramebufferInfo& info);
        [[nodiscard]] MaterialHandle            create_material(const std::string& name, const MaterialInfo& info);
        [[nodiscard]] MeshHandle                create_mesh(const std::string& name, BufferHandle vertex_buffer, BufferHandle index_buffer);
        [[nodiscard]] PipelineHandle            create_pipeline(const std::string& name, const PipelineInfo& info);
        [[nodiscard]] PipelineHandle            create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info);
        [[nodiscard]] ShaderHandle              create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type);
        [[nodiscard]] ShaderSetHandle           create_shader_set(const std::string& name, const ShaderSetInfo& info);
        [[nodiscard]] SubPassHandle             create_sub_pass(const std::string& name, ShaderSetHandle ss_h, RenderPassHandle rp_h, uint32_t index);
        [[nodiscard]] TextureHandle             create_texture(const std::string& name, const TextureInfo& info);
        [[nodiscard]] RenderPassHandle          create_render_pass(const std::string& name, const RenderPassInfo& info);

        void destroy_pipeline(PipelineHandle handle);

        [[nodiscard]] GPUBuffer*      get_buffer(BufferHandle handle) const;
        [[nodiscard]] DescriptorSet*  get_descriptor_set(DescriptorSetHandle handle) const;
        [[nodiscard]] Framebuffer*    get_framebuffer(FramebufferHandle handle) const;
        [[nodiscard]] Material*       get_material(MaterialHandle handle) const;
        [[nodiscard]] Mesh*           get_mesh(MeshHandle handle) const;
        [[nodiscard]] Pipeline*       get_pipeline(PipelineHandle handle) const;
        [[nodiscard]] PipelineLayout* get_pipeline_layout(PipelineLayoutHandle handle) const;
        [[nodiscard]] RenderPass*     get_render_pass(RenderPassHandle handle) const;
        [[nodiscard]] ShaderSet*      get_shader_set(ShaderSetHandle handle) const;
        [[nodiscard]] SubPass*        get_sub_pass(SubPassHandle handle) const;
        [[nodiscard]] GPUTexture*     get_texture(TextureHandle handle) const;

        [[nodiscard]] RenderPassHandle find_render_pass(const std::string& name) const;
        [[nodiscard]] ShaderHandle     find_shader(const std::string& name) const;
        [[nodiscard]] ShaderSetHandle  find_shader_set(const std::string& name) const;
        [[nodiscard]] TextureHandle    find_texture(const std::string& name) const;

    protected:
        std::string _resource_path;

        DescriptorPool*                       _descriptor_pool{ nullptr };
        RenderPassHandle                      _forward_render_pass{ NULL_HANDLE };
        DescriptorSetLayoutHandle             _per_view_descriptor_set_layout_h{ NULL_HANDLE };
        DescriptorSetLayoutHandle             _per_material_descriptor_set_layout_h{ NULL_HANDLE };
        std::vector<FramebufferHandle>        _forward_framebuffers;
        std::queue<std::function<void(void)>> _pre_render_queue;

        uint32_t                             _frame_counter { 0 };
        uint32_t                             _current_frame{ 0 };
        static const uint32_t                _FRAMES_IN_FLIGHT { 2 };
        std::array<FrameData, _FRAMES_IN_FLIGHT> _frame_datas;

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

        std::vector<DrawItem>          _draw_items;

        // Scene uniform data
        // TODO: Move pending scene code
        CameraData                     _camera_data;
        LightUniformData               _light_uniform_data;
        bool                           _camera_data_dirty{ false };
        bool                           _lights_dirty{ false };
};

}

#endif

