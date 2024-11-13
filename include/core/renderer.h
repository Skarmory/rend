#ifndef REND_CORE_RENDERER_H
#define REND_CORE_RENDERER_H

#include "core/containers/data_array.h"
#include "core/containers/data_pool.h"
#include "core/descriptor_set_layout.h"
#include "core/draw_pass.h"
#include "core/frame.h"
#include "core/material.h"
#include "core/mesh.h"
#include "core/presentation_mode.h"
#include "core/render_strategy.h"
#include "core/shader_set.h"
#include "core/sub_pass.h"
#include "core/view.h"

#include <functional>
#include <queue>
#include <string>
#include <vector>

namespace rend
{

class DescriptorPool;
class DescriptorSet;
class DrawPass;
class Framebuffer;
class GPUBuffer;
class GPUTexture;
class Pipeline;
class PipelineLayout;
class RenderPass;
class RenderStrategy;
class Shader;
class Window;
struct BufferInfo;
struct DescriptorSetInfo;
struct FramebufferInfo;
struct PipelineInfo;
struct PipelineLayoutInfo;
struct RenderPassInfo;
struct RendInitInfo;
struct SubPassInfo;
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
    //void add_point_light(glm::vec3 position, const PointLight& light);
    //void set_camera(const CameraData& camera);
    PresentationMode get_presentation_mode(void) const;

    virtual void configure(void) = 0;
    virtual void start_frame(void) = 0;
    virtual void end_frame(void) = 0;
    //virtual void resize(void) = 0;
    virtual void get_size_by_ratio(SizeRatio size_ratio, uint32_t& width, uint32_t& height) = 0;
    virtual void load_buffer(GPUBuffer& buffer) = 0;
    virtual void load_texture(GPUTexture& texture) = 0;

    [[nodiscard]] virtual GPUBuffer*           get_buffer(const std::string& name) const = 0;
    [[nodiscard]] virtual DescriptorSetLayout* get_descriptor_set_layout(const std::string& name) const = 0;
    [[nodiscard]] virtual Pipeline*            get_pipeline(const std::string& name) const = 0;
    [[nodiscard]] virtual RenderPass*          get_render_pass(const std::string& name) const = 0;
    [[nodiscard]] virtual RenderStrategy*      get_render_strategy(const std::string& name) const = 0;
    [[nodiscard]]         ShaderSet*           get_shader_set(const std::string& name) const;
    [[nodiscard]] virtual GPUTexture*          get_texture(const std::string& name) const = 0;

    [[nodiscard]] virtual GPUBuffer*           create_buffer(const std::string& name, const BufferInfo& info) = 0;
    [[nodiscard]] virtual DescriptorSet*       create_descriptor_set(const std::string& name, const DescriptorSetLayout& layout) = 0;
    [[nodiscard]] virtual DescriptorSetLayout* create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info) = 0;
    //[[nodiscard]]         DrawPass*            create_draw_pass(const std::string& name, const DrawPassInfo& info);
                  virtual void                 create_framebuffer(const std::string& name, const FramebufferInfo& info) = 0;
    [[nodiscard]]         Material*            create_material(const std::string& name, const MaterialInfo& info);
    [[nodiscard]]         Mesh*                create_mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer);
    [[nodiscard]] virtual Pipeline*            create_pipeline(const std::string& name, const PipelineInfo& info) = 0;
    [[nodiscard]] virtual PipelineLayout*      create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info) = 0;
    [[nodiscard]] virtual RenderPass*          create_render_pass(const std::string& name, const RenderPassInfo& info) = 0;
    [[nodiscard]]         RenderStrategy*      create_render_strategy(const std::string& name, const RenderStrategyInfo& info);
                  virtual void                 create_render_target(const std::string& name, const TextureInfo& info) = 0;
    [[nodiscard]] virtual Shader*              create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type) = 0;
    [[nodiscard]]         ShaderSet*           create_shader_set(const std::string& name, const ShaderSetInfo& info);
    //[[nodiscard]]         SubPass*             create_sub_pass(const std::string& name, const SubPassInfo& info);
    [[nodiscard]] virtual GPUTexture*          create_texture(const std::string& name, const TextureInfo& info) = 0;
    [[nodiscard]]         View*                create_view(const std::string& name, const ViewInfo& info);

                  virtual void                 destroy_buffer(GPUBuffer* buffer) = 0;
                  virtual void                 destroy_descriptor_set(DescriptorSet* set) = 0;
                  virtual void                 destroy_descriptor_set_layout(DescriptorSetLayout* layout) = 0;
                          //void                 destroy_draw_pass(DrawPass* draw_pass);
                  virtual void                 destroy_framebuffer(Framebuffer* framebuffer) = 0;
                          void                 destroy_material(Material* material);
                          void                 destroy_mesh(Mesh* mesh);
                  virtual void                 destroy_pipeline(Pipeline* pipeline) = 0;
                  virtual void                 destroy_pipeline_layout(PipelineLayout* layout) = 0;
                  virtual void                 destroy_render_pass(RenderPass* render_pass) = 0;
                          void                 destroy_render_strategy(RenderStrategy* render_strategy);
                  virtual void                 destroy_shader(Shader* shader) = 0;
                          void                 destroy_shader_set(ShaderSet* shader_set);
                          //void                 destroy_sub_pass(SubPass* sub_pass);
                  virtual void                 destroy_texture(GPUTexture* texture) = 0;
                          void                 destroy_view(View* view);

protected:
    virtual ~Renderer(void);
    virtual void _resize(void) = 0;

protected:
    static constexpr std::string C_BACKBUFFER_NAME = "backbuffer";

    Window*                               _window{ nullptr };
    std::queue<std::function<void(void)>> _pre_render_queue;

    uint32_t                             _frame_counter { 0 };
    uint32_t                             _current_frame{ 0 };
    static const uint32_t                _FRAMES_IN_FLIGHT { 2 };
    PresentationMode _presentation_mode{ PresentationMode::DOUBLE_BUFFERING };
    std::array<FrameData, _FRAMES_IN_FLIGHT> _frame_datas;
    bool _need_resize{ false };

    //DataArray<DrawPass> _draw_passes;
    DataArray<Material> _materials;
    DataArray<Mesh> _meshes;
    DataArray<RenderStrategy> _render_strategies;
    DataArray<ShaderSet> _shader_sets;
    //DataArray<SubPass> _sub_passes;
    DataArray<View> _views;

    std::vector<DrawItem> _draw_items;

    static Renderer* _renderer;
};

}

#endif

