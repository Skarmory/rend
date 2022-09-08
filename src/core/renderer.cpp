#include "core/renderer.h"

#include "core/descriptor_set.h"
#include "core/descriptor_pool.h"

using namespace rend;

Renderer::Renderer(const std::string& resource_path)
    :
        _resource_path(resource_path)
{
}

void Renderer::add_draw_item(const DrawItem& item)
{
    _draw_items.push_back(item);
}

void Renderer::add_pre_render_task(std::function<void(void)> func)
{
    _pre_render_queue.push(func);
}

void Renderer::add_point_light(glm::vec3 position, const PointLight& light)
{
    _light_uniform_data.light_positions[_light_uniform_data.light_count] = glm::vec4(position, 0.0f);
    _light_uniform_data.light_data[_light_uniform_data.light_count] = light;
    ++_light_uniform_data.light_count;
    _lights_dirty = true;
}

void Renderer::set_camera(const CameraData& camera)
{
    _camera_data = camera;
    _camera_data_dirty = true;
}

BufferHandle Renderer::create_buffer(const std::string& name, const BufferInfo& info)
{
    return _gpu_buffers.allocate(name, info);
}

DescriptorSetHandle Renderer::create_descriptor_set(DescriptorSetLayoutHandle layout_handle, uint32_t set_number)
{
    DescriptorSetInfo info =
    {
        .layout_handle = layout_handle,
        .set_number = set_number
    };

    return _descriptor_pool->allocate_descriptor_set(info);
}

DescriptorSetLayoutHandle Renderer::create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info)
{
    return _desc_set_layouts.allocate(name, info);
}

FramebufferHandle Renderer::create_framebuffer(const std::string& name, const FramebufferInfo& info)
{
    FramebufferInfo api_info = info;
    api_info.render_pass = _render_passes.get(info.render_pass)->handle();

    if(info.depth_target != NULL_HANDLE)
    {
        api_info.depth_target = _gpu_textures.get(info.depth_target)->handle();
    }

    for(size_t idx = 0; idx < api_info.render_targets.size(); ++idx)
    {
        api_info.render_targets[idx] = _gpu_textures.get(info.render_targets[idx])->handle();
    }

    return _framebuffers.allocate(name, info, api_info);
}

MaterialHandle Renderer::create_material(const std::string& name, const MaterialInfo& info)
{
    return _materials.allocate(name, info);
}

MeshHandle Renderer::create_mesh(const std::string& name, BufferHandle vertex_buffer, BufferHandle index_buffer)
{
    return _meshes.allocate(name, vertex_buffer, index_buffer);
}

PipelineHandle Renderer::create_pipeline(const std::string& name, const PipelineInfo& info)
{
    return _pipelines.allocate(name, info);
}

PipelineHandle Renderer::create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info)
{
    return _pipeline_layouts.allocate(name, info);
}

RenderPassHandle Renderer::create_render_pass(const std::string& name, const RenderPassInfo& info)
{
    return _render_passes.allocate(name, info);
}

void Renderer::destroy_pipeline(PipelineHandle handle)
{
    _pipelines.deallocate(handle);
}

ShaderHandle Renderer::create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type)
{
    return _shaders.allocate(name, code, size_bytes, type);
}

ShaderSetHandle Renderer::create_shader_set(const std::string& name, const ShaderSetInfo& info)
{
    return _shader_sets.allocate(name, info);
}

SubPassHandle Renderer::create_sub_pass(const std::string& name, ShaderSetHandle ss_h, RenderPassHandle rp_h, uint32_t index)
{
    return _subpasses.allocate(name, ss_h, rp_h, index);
}

TextureHandle Renderer::create_texture(const std::string& name, const TextureInfo& info)
{
    return _gpu_textures.allocate(name, info);
}

GPUBuffer* Renderer::get_buffer(BufferHandle handle) const
{
    return _gpu_buffers.get(handle);
}

DescriptorSet* Renderer::get_descriptor_set(DescriptorSetHandle handle) const
{
    return _descriptor_pool->get_descriptor_set(handle);
}

Framebuffer* Renderer::get_framebuffer(FramebufferHandle handle) const
{
    return _framebuffers.get(handle);
}

Material* Renderer::get_material(MaterialHandle handle) const
{
    return _materials.get(handle);
}

Mesh* Renderer::get_mesh(MeshHandle handle) const
{
    return _meshes.get(handle);
}

Pipeline* Renderer::get_pipeline(PipelineHandle handle) const
{
    return _pipelines.get(handle);
}

PipelineLayout* Renderer::get_pipeline_layout(PipelineLayoutHandle handle) const
{
    return _pipeline_layouts.get(handle);
}

RenderPass* Renderer::get_render_pass(RenderPassHandle handle) const
{
    return _render_passes.get(handle);
}

ShaderSet* Renderer::get_shader_set(ShaderSetHandle handle) const
{
    return _shader_sets.get(handle);
}

SubPass* Renderer::get_sub_pass(SubPassHandle handle) const
{
    return _subpasses.get(handle);
}

GPUTexture* Renderer::get_texture(TextureHandle handle) const
{
    return _gpu_textures.get(handle);
}

RenderPassHandle Renderer::find_render_pass(const std::string& name) const
{
    auto id = std::hash<std::string>{}(name);
    return _render_passes.find([&id](const RenderPass& rp) { return id == rp.id(); });
}

ShaderHandle Renderer::find_shader(const std::string& name) const
{
    auto id = std::hash<std::string>{}(name);
    return _shaders.find([&id](const Shader& s) { return id == s.id(); });
}

ShaderSetHandle Renderer::find_shader_set(const std::string& name) const
{
    auto id = std::hash<std::string>{}(name);
    return _shader_sets.find([&id](const ShaderSet& ss) { return id == ss.id(); });
}

TextureHandle Renderer::find_texture(const std::string& name) const
{
    auto id = std::hash<std::string>{}(name);
    return _gpu_textures.find([&id](const GPUTexture& t) { return id == t.id();  } );
}
