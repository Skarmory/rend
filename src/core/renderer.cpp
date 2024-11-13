#include "core/renderer.h"

#include "core/descriptor_set.h"
#include "core/descriptor_pool.h"
#include "core/rend.h"
#include "core/window.h"

#include "api/vulkan/vulkan_renderer.h"

#include <assert.h>

using namespace rend;

Renderer* Renderer::_renderer =  nullptr;

void Renderer::initialise(const RendInitInfo& init_info)
{
    assert(_renderer == nullptr);

    switch(init_info.api)
    {
        case API::API_VULKAN:
            _renderer = new VulkanRenderer(init_info);
            break;
    }
}

Renderer::~Renderer(void)
{

}

void Renderer::shutdown(void)
{
    delete _renderer;
}

Renderer& Renderer::get_instance(void)
{
    assert(_renderer != nullptr);
    return *_renderer;
}

Window* Renderer::get_window(void) const
{
    return _window;
}

void Renderer::add_draw_item(const DrawItem& item)
{
    _draw_items.push_back(item);
}

void Renderer::add_pre_render_task(std::function<void(void)> func)
{
    _pre_render_queue.push(func);
}

//void Renderer::add_point_light(glm::vec3 position, const PointLight& light)
//{
//    _light_uniform_data.light_positions[_light_uniform_data.light_count] = glm::vec4(position, 0.0f);
//    _light_uniform_data.light_data[_light_uniform_data.light_count] = light;
//    ++_light_uniform_data.light_count;
//    _lights_dirty = true;
//}

//void Renderer::set_camera(const CameraData& camera)
//{
//    _camera_data = camera;
//    _camera_data_dirty = true;
//}

//DrawPass* Renderer::create_draw_pass(const std::string& name, const DrawPassInfo& info)
//{
//    auto rend_handle = _draw_passes.allocate(name, info);
//    auto* draw_pass = _draw_passes.get(rend_handle);
//    draw_pass->_rend_handle = rend_handle;
//    return draw_pass;
//}

Material* Renderer::create_material(const std::string& name, const MaterialInfo& info)
{
    //std::stringstream ss;
    //ss << "Material " << name << " descriptor set";

    // TODO: sort out multiple draw passes and multiple subpasses
    //auto* draw_pass = info.render_strategy->get_draw_passes()[0];
    //auto* subpass = draw_pass->get_subpasses()[0];

    //DescriptorSetInfo ds_info{};
    //ds_info.layout = &subpass->get_shader_set().get_descriptor_set_layout(DescriptorFrequency::MATERIAL);
    //ds_info.set    = DescriptorFrequency::MATERIAL;

    //auto* descriptor_set = static_cast<VulkanDescriptorSet*>(create_descriptor_set(ss.str(), ds_info));

    //if(info.albedo_texture != nullptr)
    //{
    //    descriptor_set->add_texture_binding((int)MaterialBindingSlot::ALBEDO, info.albedo_texture);
    //}

    //write_descriptor_bindings(descriptor_set);

    auto rend_handle = _materials.allocate(name, info);
    auto* material = _materials.get(rend_handle);
    material->_rend_handle = rend_handle;
    return material;
}

Mesh* Renderer::create_mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer)
{
    auto rend_handle = _meshes.allocate(name, vertex_buffer, index_buffer);
    auto* mesh = _meshes.get(rend_handle);
    mesh->_rend_handle = rend_handle;
    return mesh;
}

RenderStrategy* Renderer::create_render_strategy(const std::string& name, const RenderStrategyInfo& info)
{
    auto rend_handle = _render_strategies.allocate(name, info);
    auto* render_strategy = _render_strategies.get(rend_handle);
    static_cast<RendObject*>(render_strategy)->_rend_handle = rend_handle;
    return render_strategy;
}

ShaderSet* Renderer::create_shader_set(const std::string& name, const ShaderSetInfo& info)
{
    std::vector<DescriptorSetLayout*> layouts;
    for(size_t i = 0; i < info.layouts.size(); ++i)
    {
        if(info.layouts[i] != nullptr)
        {
            layouts.push_back(info.layouts[i]);
        }
    }

    PipelineLayoutInfo pl_info{};
    pl_info.descriptor_set_layouts = layouts;
    pl_info.push_constant_ranges = info.push_constant_ranges;

    auto* pipeline_layout = create_pipeline_layout(name + " pipeline layout", pl_info);
    auto rend_handle = _shader_sets.allocate(name, info, pipeline_layout);
    auto* rend_shader_set = _shader_sets.get(rend_handle);
    rend_shader_set->_rend_handle = rend_handle;

    return rend_shader_set;
}

//SubPass* Renderer::create_sub_pass(const std::string& name, const SubPassInfo& info)
//{
//    //PipelineInfo pl_info{};
//    //pl_info.shader_set = info.shader_set;
//    //pl_info.shaders[0] = &info.shader_set->get_shader(ShaderIndex::SHADER_INDEX_VERTEX);
//    //pl_info.shaders[1] = &info.shader_set->get_shader(ShaderIndex::SHADER_INDEX_FRAGMENT);
//    //pl_info.vertex_binding_info.index = 0;
//    //pl_info.vertex_binding_info.stride = 32; // TODO: Figure out how to work out vertex bindings properly
//
//    //const auto& va_infos = info.shader_set->get_vertex_attribute_infos();
//    //pl_info.vertex_attribute_info_count = va_infos.size();
//
//    //for(size_t i = 0; i < va_infos.size(); ++i)
//    //{
//    //    pl_info.vertex_attribute_infos[i] = va_infos[i];
//    //    pl_info.vertex_attribute_infos[i].binding = &pl_info.vertex_binding_info;
//    //}
//
//    // TODO: Make this dynamic
//    //pl_info.colour_blending_info.blend_attachments[0].blend_enabled = true;
//    //pl_info.colour_blending_info.blend_attachments[0].colour_src_factor = BlendFactor::ONE;
//    //pl_info.colour_blending_info.blend_attachments[0].colour_dst_factor = BlendFactor::ZERO;
//    //pl_info.colour_blending_info.blend_attachments_count = 1;
//
//    //pl_info.dynamic_states = DynamicState::VIEWPORT | DynamicState::SCISSOR;
//    //pl_info.viewport_info_count = 1;
//    //pl_info.scissor_info_count = 1;
//
//    //pl_info.layout = &info.shader_set->get_pipeline_layout();
//    //pl_info.render_pass = info.render_pass;
//
//    //Pipeline* pipeline = create_pipeline("Pipeline: " + name, pl_info);
//
//    auto rend_handle = _sub_passes.allocate(name, info);
//    auto* sub_pass = _sub_passes.get(rend_handle);
//    sub_pass->_rend_handle = rend_handle;
//    return sub_pass;
//}

View* Renderer::create_view(const std::string& name, const ViewInfo& info)
{
    auto rend_handle = _views.allocate(name, info);
    auto* view = _views.get(rend_handle);
    view->_rend_handle = rend_handle;
    return view;
}

//void Renderer::destroy_draw_pass(DrawPass* draw_pass)
//{
//    auto rend_handle = draw_pass->rend_handle();
//    _draw_passes.deallocate(rend_handle);
//}

void Renderer::destroy_material(Material* material)
{
    auto rend_handle = material->rend_handle();
    _materials.deallocate(rend_handle);
}

void Renderer::destroy_mesh(Mesh* mesh)
{
    auto rend_handle = mesh->rend_handle();
    _meshes.deallocate(rend_handle);
}

void Renderer::destroy_render_strategy(RenderStrategy* render_strategy) 
{
    auto rend_handle = render_strategy->rend_handle();
    _render_strategies.deallocate(rend_handle);
}

void Renderer::destroy_shader_set(ShaderSet* shader_set)
{
    auto rend_handle = shader_set->rend_handle();
    _shader_sets.deallocate(rend_handle);
}

//void Renderer::destroy_sub_pass(SubPass* sub_pass)
//{
//    auto rend_handle = sub_pass->rend_handle();
//    _sub_passes.deallocate(rend_handle);
//}

void Renderer::destroy_view(View* view)
{
    auto rend_handle = view->rend_handle();
    _views.deallocate(rend_handle);
}

PresentationMode Renderer::get_presentation_mode(void) const
{
    return _presentation_mode;
}

//DrawPass* Renderer::get_draw_pass(const std::string& name) const
//{
//    for(const DrawPass& draw_pass : _draw_passes)
//    {
//        if(draw_pass.name() == name)
//        {
//            return const_cast<DrawPass*>(&draw_pass);
//        }
//    }
//
//    return nullptr;
//}

ShaderSet* Renderer::get_shader_set(const std::string& name) const
{
    for(const ShaderSet& shader_set : _shader_sets)
    {
        if(shader_set.name() == name)
        {
            return const_cast<ShaderSet*>(&shader_set);
        }
    }

    return nullptr;
}
