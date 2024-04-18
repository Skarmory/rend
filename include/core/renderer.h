#ifndef REND_CORE_RENDERER_H
#define REND_CORE_RENDERER_H

#include "core/descriptor_set_layout.h"
#include "core/camera.h"
#include "core/frame.h"
#include "core/material.h"
#include "core/mesh.h"
#include "core/shader_set.h"
#include "core/sub_pass.h"
#include "core/containers/data_array.h"
#include "core/containers/data_pool.h"
#include "lights/point_light.h"
#include "lights/light_uniform_data.h"
#include <functional>
#include <queue>
#include <string>
#include <vector>

namespace rend
{

class DescriptorPool;
class DescriptorSet;
class Framebuffer;
class GPUBuffer;
class GPUTexture;
class Pipeline;
class PipelineLayout;
class RenderPass;
class Shader;
class Window;
struct BufferInfo;
struct DescriptorSetInfo;
struct FramebufferInfo;
struct PipelineInfo;
struct PipelineLayoutInfo;
struct RenderPassInfo;
struct RendInitInfo;
struct TextureInfo;

class Renderer
{
public:
    static void initialise(const RendInitInfo& init_info);
    static void shutdown(void);
    static Renderer& get_instance(void);

    Window* get_window(void) const;

    void add_draw_item(const DrawItem& draw_item);
    void add_pre_render_task(std::function<void(void)> task_func);
    void add_point_light(glm::vec3 position, const PointLight& light);
    void set_camera(const CameraData& camera);

    virtual void configure(void) = 0;
    virtual void start_frame(void) = 0;
    virtual void end_frame(void) = 0;
    //virtual void resize(void) = 0;
    virtual void get_size_by_ratio(SizeRatio size_ratio, uint32_t& width, uint32_t& height) = 0;
    virtual void load_buffer(GPUBuffer& buffer, const void* data, size_t bytes, uint32_t offset) = 0;
    virtual void load_texture(GPUTexture& texture, const void* data, size_t bytes, uint32_t offset) = 0;

    RenderPass* get_render_pass(void) const;

    [[nodiscard]] virtual GPUBuffer*           create_buffer(const std::string& name, const BufferInfo& info) = 0;
    [[nodiscard]] virtual DescriptorSet*       create_descriptor_set(const std::string& name, const DescriptorSetInfo& info) = 0;
    [[nodiscard]] virtual DescriptorSetLayout* create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info) = 0;
    [[nodiscard]] virtual Framebuffer*         create_framebuffer(const std::string& name, const FramebufferInfo& info) = 0;
    [[nodiscard]] virtual Material*            create_material(const std::string& name, const MaterialInfo& info) = 0;
    [[nodiscard]] virtual Mesh*                create_mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer) = 0;
    [[nodiscard]] virtual Pipeline*            create_pipeline(const std::string& name, const PipelineInfo& info) = 0;
    [[nodiscard]] virtual PipelineLayout*      create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info) = 0;
    [[nodiscard]] virtual RenderPass*          create_render_pass(const std::string& name, const RenderPassInfo& info) = 0;
    [[nodiscard]] virtual Shader*              create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type) = 0;
    [[nodiscard]] virtual ShaderSet*           create_shader_set(const std::string& name, const ShaderSetInfo& info) = 0;
    [[nodiscard]] virtual SubPass*             create_sub_pass(const std::string& name, const SubPassInfo& info) = 0;
    [[nodiscard]] virtual GPUTexture*          create_texture(const std::string& name, const TextureInfo& info) = 0;

    virtual void destroy_buffer(GPUBuffer* buffer) = 0;
    virtual void destroy_descriptor_set(DescriptorSet* set) = 0;
    virtual void destroy_descriptor_set_layout(DescriptorSetLayout* layout) = 0;
    virtual void destroy_framebuffer(Framebuffer* framebuffer) = 0;
    virtual void destroy_material(Material* material) = 0;
    virtual void destroy_mesh(Mesh* mesh) = 0;
    virtual void destroy_pipeline(Pipeline* pipeline) = 0;
    virtual void destroy_pipeline_layout(PipelineLayout* layout) = 0;
    virtual void destroy_render_pass(RenderPass* render_pass) = 0;
    virtual void destroy_shader(Shader* shader) = 0;
    virtual void destroy_shader_set(ShaderSet* shader_set) = 0;
    virtual void destroy_sub_pass(SubPass* sub_pass) = 0;
    virtual void destroy_texture(GPUTexture* texture) = 0;

protected:
    Renderer(const RendInitInfo& init_info);
    virtual ~Renderer(void);

protected:
    std::string _resource_path;

    Window*                               _window{ nullptr };
    RenderPass*                           _forward_render_pass{ nullptr };
    DescriptorSetLayout*                  _per_view_descriptor_set_layout{ nullptr };
    DescriptorSetLayout*                  _per_material_descriptor_set_layout{ nullptr };
    std::vector<Framebuffer*>             _forward_framebuffers;
    std::queue<std::function<void(void)>> _pre_render_queue;

    uint32_t                             _frame_counter { 0 };
    uint32_t                             _current_frame{ 0 };
    static const uint32_t                _FRAMES_IN_FLIGHT { 2 };
    std::array<FrameData, _FRAMES_IN_FLIGHT> _frame_datas;

    DataPool<ShaderSet, 32> _shader_sets;
    DataPool<SubPass, 32>   _sub_passes;
    DataPool<Material, 32>  _materials;
    DataPool<Mesh, 32>      _meshes;

    std::vector<DrawItem>          _draw_items;

    // Scene uniform data
    // TODO: Move pending scene code
    CameraData                     _camera_data;
    LightUniformData               _light_uniform_data;
    bool                           _camera_data_dirty{ false };
    bool                           _lights_dirty{ false };

    static Renderer* _renderer;
};

}

#endif

