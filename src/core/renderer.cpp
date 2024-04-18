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

void Renderer::shutdown(void)
{
    delete _renderer;
}

Renderer& Renderer::get_instance(void)
{
    assert(_renderer != nullptr);
    return *_renderer;
}

Renderer::Renderer(const RendInitInfo& init_info)
    :
        _resource_path(init_info.resource_path)
{
}

Renderer::~Renderer(void)
{
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

RenderPass* Renderer::get_render_pass(void) const
{
    // TODO: temporary
    return _forward_render_pass;
}
