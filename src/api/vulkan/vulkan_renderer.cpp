#include "api/vulkan/vulkan_renderer.h"

#include "core/command_buffer.h"
#include "core/descriptor_set.h"
#include "core/descriptor_pool.h"
#include "core/device_context.h"
#include "core/material.h"
#include "core/rend.h"
#include "core/rend_defs.h"
#include "core/sub_pass.h"
#include "core/window.h"

#include "api/vulkan/fence.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/swapchain.h"
#include "api/vulkan/vulkan_command_buffer.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_semaphore.h"

#include <assert.h>
#include <cstring>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace rend;

namespace
{
    // TODO: move glfw specific code to a glfw wrapper

    void glfw_window_resize_callback(GLFWwindow* window, int width, int height)
    {
        (void)window;
        auto& rr = static_cast<VulkanRenderer&>(Renderer::get_instance());
        rr.get_window()->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        rr.resize();
    }

    void glfw_error_callback(int error, const char* description)
    {
        if(error == 65539)
        {
            return;
        }

        std::cerr << "GLFW error code: " << error << std::endl << description << std::endl;
    }
}

VulkanRenderer::VulkanRenderer(const RendInitInfo& init_info)
    :
        Renderer(init_info)
{
    glfwInit();

    _window = new Window(init_info.resolution_width, init_info.resolution_height, init_info.app_name);
    glfwSetInputMode(_window->get_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(_window->get_handle(), ::glfw_window_resize_callback);
    glfwSetErrorCallback(::glfw_error_callback);

    _device_context = new VulkanDeviceContext(*static_cast<VulkanInitInfo*>(init_info.api_init_info), *_window);
    _swapchain = new Swapchain(3, *_device_context);

    DescriptorPoolSize pool_sizes[] =
    {
        { DescriptorType::UNIFORM_BUFFER, 32 },
        { DescriptorType::COMBINED_IMAGE_SAMPLER, 32 }
    };

    DescriptorPoolInfo pool_info =
    {
        .pool_sizes = pool_sizes,
        .pool_sizes_count = 2,
        .max_sets = 9
    };

    _descriptor_pool = _device_context->create_descriptor_pool(pool_info);
    _command_pool = _device_context->create_command_pool();

    {
        size_t bytes = 32 * 1080 * 1080;
        BufferInfo info{ 1, bytes, BufferUsage::TRANSFER_SRC };

        // TODO Improve buffer creation to avoid this
        std::vector<BufferHandle> temp_buffers;
        for(int i = 0; i < _staging_buffers.capacity(); ++i)
        {
            std::stringstream ss;
            ss << "Staging buffer " << i;

            VulkanBufferInfo vk_info = _device_context->create_buffer(info, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            auto buffer_handle = _staging_buffers.acquire();
            VulkanBuffer* buffer = _staging_buffers.get(buffer_handle);
            buffer = new(buffer) VulkanBuffer(ss.str(), buffer_handle, info, vk_info);
            temp_buffers.push_back(buffer_handle);
        }

        for(int i = 0; i < _staging_buffers.capacity(); ++i)
        {
            _staging_buffers.release(temp_buffers[i]);
        }
    }
}

VulkanRenderer::~VulkanRenderer(void)
{
    _device_context->get_device()->wait_idle();

    for(VulkanBuffer& buffer : _staging_buffers)
    {
        destroy_buffer(&buffer);
    }

    for(VulkanBuffer& buffer : _buffers)
    {
        destroy_buffer(&buffer);
    }

    for(VulkanTexture& texture : _textures)
    {
        destroy_texture(&texture);
    }

    for(VulkanFramebuffer& framebuffer : _framebuffers)
    {
        destroy_framebuffer(&framebuffer);
    }

    for(VulkanRenderPass& render_pass : _render_passes)
    {
        destroy_render_pass(&render_pass);
    }

    for(VulkanShader& shader : _shaders)
    {
        destroy_shader(&shader);
    }

    for(VulkanPipeline& pipeline : _pipelines)
    {
        destroy_pipeline(&pipeline);
    }

    for(VulkanPipelineLayout& layout : _pipeline_layouts)
    {
        destroy_pipeline_layout(&layout);
    }

    for(VulkanDescriptorSet& set : _descriptor_sets)
    {
        destroy_descriptor_set(&set);
    }

    for(VulkanDescriptorSetLayout& layout : _descriptor_set_layouts)
    {
        destroy_descriptor_set_layout(&layout);
    }

    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        delete _frame_datas[idx].acquire_sem;
        delete _frame_datas[idx].present_sem;
        delete _frame_datas[idx].submit_fen;
        delete _frame_datas[idx].other_sem;
    }

    _device_context->destroy_command_pool(_command_pool);
    _device_context->destroy_descriptor_pool(_descriptor_pool);
    delete _swapchain;
    delete _device_context;
    delete _window;

    glfwTerminate();
}

void VulkanRenderer::configure(void)
{
    _setup_forward();
}

void VulkanRenderer::start_frame(void)
{
    // Update frame and grab next
    _current_frame = ++_frame_counter % _FRAMES_IN_FLIGHT;
    FrameData& frame_res = _frame_datas[_current_frame];
    frame_res.frame = _frame_counter;

    // Release command buffers from previous usage of this FrameData
    frame_res.submit_fen->wait();
    frame_res.command_buffer->reset();
    frame_res.other_buffer->reset();

    // Release staging buffers from previous usage of this FrameData
    for(auto* staging_buffer : frame_res.staging_buffers_used)
    {
        _staging_buffers.release(staging_buffer->rend_handle());
    }

    frame_res.staging_buffers_used.clear();
}

void VulkanRenderer::end_frame(void)
{
    FrameData& frame_res = _frame_datas[_current_frame];

    // Acquire next swapchain image
    StatusCode code = StatusCode::SUCCESS;
    while((code = _swapchain->acquire(frame_res.acquire_sem, nullptr)) != StatusCode::SUCCESS)
    {
        if(code == StatusCode::SWAPCHAIN_ACQUIRE_ERROR)
        {
            std::cerr << "Failed to acquire swapchain image" << std::endl;
            std::abort();
        }

        if(code == StatusCode::SWAPCHAIN_OUT_OF_DATE)
        {
            resize();
        }
    }

    // Set new swapchain image to render into
    frame_res.swapchain_idx = _swapchain->get_current_image_index();
    frame_res.framebuffer = _forward_framebuffers[frame_res.swapchain_idx];

    auto* other_cmd = static_cast<VulkanCommandBuffer*>(frame_res.other_buffer);
    other_cmd->begin();
    {
        _process_pre_render_tasks();
    }
    other_cmd->end();

    if(other_cmd->recorded())
    {
        VkCommandBuffer vk_command_buffer = other_cmd->vk_handle();
        _device_context->get_device()->queue_submit(&vk_command_buffer, 1, QueueType::GRAPHICS, { }, { frame_res.other_sem }, nullptr);
    }

    auto* vk_cmd = static_cast<VulkanCommandBuffer*>(frame_res.command_buffer);
    vk_cmd->begin();
    {
        _update_uniform_buffers();
        _process_draw_items();
    }
    vk_cmd->end();

    if(vk_cmd->recorded())
    {
        frame_res.submit_fen->reset();
        VkCommandBuffer vk_command_buffer = vk_cmd->vk_handle();
        _device_context->get_device()->queue_submit(&vk_command_buffer, 1, QueueType::GRAPHICS, { frame_res.acquire_sem, frame_res.other_sem }, { frame_res.present_sem }, frame_res.submit_fen);
        _swapchain->present(QueueType::GRAPHICS, { frame_res.present_sem });
    }
}

void VulkanRenderer::resize(void)
{
    for(auto* fb : _forward_framebuffers)
    {
        for(auto rt : fb->get_colour_attachments())
        {
            destroy_texture(rt);
        }

        if(fb->get_depth_stencil_attachment() != nullptr)
        {
            destroy_texture(fb->get_depth_stencil_attachment());
        }

        destroy_framebuffer(fb);
    }

    _forward_framebuffers.clear();

    _swapchain->recreate();
    _create_framebuffers();
}

void VulkanRenderer::get_size_by_ratio(SizeRatio size_ratio, uint32_t& width, uint32_t& height)
{
    VkExtent2D extent = _swapchain->get_extent();

    if(size_ratio == SizeRatio::FULL)
    {
        width = extent.width;
        height = extent.height;
    }
    else if(size_ratio == SizeRatio::HALF)
    {
        width = extent.width / 2;
        height = extent.height / 2;
    }
}

void VulkanRenderer::load_texture(GPUTexture& texture, const void* data, size_t bytes, uint32_t offset)
{
    transition(texture, PipelineStage::PIPELINE_STAGE_TOP_OF_PIPE, PipelineStage::PIPELINE_STAGE_TRANSFER, ImageLayout::TRANSFER_DST);

    FrameData& fr = _frame_datas[_current_frame];
    CommandBuffer* cmd = fr.other_buffer;
    void* mapped = NULL;

    auto  staging_buffer_h = _staging_buffers.acquire();
    VulkanBuffer* staging_buffer = _staging_buffers.get(staging_buffer_h);

    mapped = _device_context->map_buffer_memory(*staging_buffer, staging_buffer->bytes());
    memcpy(mapped, data, bytes);
    _device_context->unmap_buffer_memory(*staging_buffer);

    BufferImageCopyInfo info =
    {
        .buffer_offset  = 0,
        .buffer_width   = texture.width(),
        .buffer_height  = texture.height(),
        .image_offset_x = 0,
        .image_offset_y = 0,
        .image_offset_z = 0,
        .image_width    = texture.width(),
        .image_height   = texture.height(),
        .image_depth    = texture.depth(),
        .image_layout   = texture.layout(),
        .mip_level      = 0,
        .base_layer     = 0,
        .layer_count    = texture.layers()
    };

    cmd->copy(*staging_buffer, texture, info);
    transition(texture, PipelineStage::PIPELINE_STAGE_TRANSFER, PipelineStage::PIPELINE_STAGE_FRAGMENT_SHADER, ImageLayout::SHADER_READ_ONLY);

    fr.staging_buffers_used.push_back(staging_buffer);
}

void VulkanRenderer::load_buffer(GPUBuffer& buffer, const void* data, size_t bytes, uint32_t offset)
{
    // TODO: This should be based on the memory properties, not the resource
    bool is_device_local{ false };
    if ((buffer.usage() & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE ||
        (buffer.usage() & BufferUsage::INDEX_BUFFER)  != BufferUsage::NONE)
    {
        is_device_local = true;
    }

    void* mapped = NULL;

    if(is_device_local)
    {
        auto staging_buffer_h = _staging_buffers.acquire();
        GPUBuffer* staging_buffer = _staging_buffers.get(staging_buffer_h);
        FrameData& fr = _frame_datas[_current_frame];
        CommandBuffer* cmd = fr.other_buffer;

        mapped = _device_context->map_buffer_memory(*staging_buffer, staging_buffer->bytes());
        memcpy(mapped, data, bytes);
        _device_context->unmap_buffer_memory(*staging_buffer);

        BufferBufferCopyInfo info =
        {
            .size_bytes = (uint32_t)buffer.bytes(),
            .src_offset = 0,
            .dst_offset = 0
        };

        cmd->copy(*staging_buffer, buffer, info);

        fr.staging_buffers_used.push_back(staging_buffer);
    }
    else
    {
        mapped = _device_context->map_buffer_memory(buffer, buffer.bytes());
        memcpy(mapped, data, bytes);
        _device_context->unmap_buffer_memory(buffer);
    }
}

void VulkanRenderer::transition(GPUTexture& texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout)
{
    FrameData& fr = _frame_datas[_current_frame];
    VulkanCommandBuffer* vk_cmd = static_cast<VulkanCommandBuffer*>(fr.other_buffer);
    vk_cmd->transition_image(texture, src, dst, final_layout);
}

void VulkanRenderer::write_descriptor_bindings(DescriptorSet* descriptor_set)
{
    auto* vk_set = static_cast<VulkanDescriptorSet*>(descriptor_set);
    auto& vk_set_info = vk_set->vk_set_info();
    _device_context->write_descriptor_bindings(vk_set_info.set, vk_set->bindings());
}

Swapchain* VulkanRenderer::get_swapchain(void) const
{
    return _swapchain;
}

VulkanDeviceContext* VulkanRenderer::device_context(void) const
{
    return _device_context;
}

Shader* VulkanRenderer::_load_shader(const std::string& name, const std::string& path, rend::ShaderStage stage)
{
    std::string full_path = _resource_path + path + name;
    std::vector<char> shader_code;
    std::ifstream fs(full_path, std::ios::binary | std::ios::ate);
    shader_code.resize(fs.tellg());
    fs.seekg(0);
    fs.read(shader_code.data(), shader_code.size());
    return create_shader(name, shader_code.data(), shader_code.size(), stage);
}

void VulkanRenderer::_create_descriptor_set_layouts(void)
{
    // Create and add per view descriptor set layout
    {
        DescriptorSetLayoutBinding camera_data_binding =
        {
            .binding = 0,
            .descriptor_type = DescriptorType::UNIFORM_BUFFER,
            .descriptor_count = 1,
            .shader_stages = ShaderStage::SHADER_STAGE_VERTEX
        };

        DescriptorSetLayoutBinding light_data_binding =
        {
            .binding = 1,
            .descriptor_type = DescriptorType::UNIFORM_BUFFER,
            .descriptor_count = 1,
            .shader_stages = ShaderStage::SHADER_STAGE_FRAGMENT
        };

        DescriptorSetLayoutInfo dsl_info{};
        dsl_info.layout_bindings.push_back(camera_data_binding);
        dsl_info.layout_bindings.push_back(light_data_binding);

        _per_view_descriptor_set_layout = create_descriptor_set_layout("per view", dsl_info);
    }

    // Create and add per material descriptor set layout
    {
        DescriptorSetLayoutBinding diffuse_texture_binding =
        {
            .binding = 0,
            .descriptor_type = DescriptorType::COMBINED_IMAGE_SAMPLER,
            .descriptor_count = 1,
            .shader_stages = ShaderStage::SHADER_STAGE_FRAGMENT
        };

        DescriptorSetLayoutInfo dsl_info{};
        dsl_info.layout_bindings.push_back(diffuse_texture_binding);

        _per_material_descriptor_set_layout = create_descriptor_set_layout("per view", dsl_info);
    }
}

ShaderSet* VulkanRenderer::_load_shader_set(const Shader& vertex_shader, const Shader& fragment_shader)
{
    ShaderSetInfo shader_set_info{};
    shader_set_info.shaders[ShaderIndex::SHADER_INDEX_VERTEX]   = &vertex_shader;
    shader_set_info.shaders[ShaderIndex::SHADER_INDEX_FRAGMENT] = &fragment_shader;
    shader_set_info.vertex_attribute_infos.push_back({ .location = 0, .offset = 0 });
    shader_set_info.vertex_attribute_infos.push_back({ .location = 1, .offset = 12 });
    shader_set_info.vertex_attribute_infos.push_back({ .location = 2, .offset = 24 });
    shader_set_info.layouts[DescriptorFrequency::VIEW] = _per_view_descriptor_set_layout;
    shader_set_info.layouts[DescriptorFrequency::MATERIAL] = _per_material_descriptor_set_layout;

    PushConstantRange range =
    {
        .shader_stages = ShaderStage::SHADER_STAGE_VERTEX | ShaderStage::SHADER_STAGE_FRAGMENT,
        .offset = 0,
        .size = 68
    };

    shader_set_info.push_constant_ranges.push_back(range);

    return create_shader_set("light", shader_set_info);
}

GPUTexture* VulkanRenderer::_create_depth_buffer(VkExtent2D extent)
{
    TextureInfo info{ extent.width, extent.height, 1, {}, 1, 1, Format::D24_S8, ImageLayout::UNDEFINED,  MSAASamples::MSAA_1X, ImageUsage::DEPTH_STENCIL };
    auto* texture = create_texture("depth buffer", info);
    return texture;
}

void VulkanRenderer::_create_framebuffers(void)
{
    VkExtent2D swapchain_extent = _swapchain->get_extent();
    auto& swapchain_images = _swapchain->get_back_buffer_textures();

    _forward_framebuffers.clear();
    _forward_framebuffers.resize(swapchain_images.size());

    TextureInfo backbuffer_info{};
    backbuffer_info.depth = 0;
    backbuffer_info.mips = 0;
    backbuffer_info.layers = 0;
    backbuffer_info.format = _swapchain->get_format();
    backbuffer_info.layout = ImageLayout::UNDEFINED;
    backbuffer_info.samples = MSAASamples::MSAA_1X;
    backbuffer_info.usage = ImageUsage::COLOUR_ATTACHMENT | ImageUsage::TRANSFER_DST;

    for(size_t idx = 0; idx < swapchain_images.size(); ++idx)
    {
        std::stringstream ss;
        ss << "Backbuffer " << idx;

        GPUTexture* colour_target = _register_swapchain_image(ss.str(), backbuffer_info, swapchain_images[idx]); 
        GPUTexture* depth_buffer = _create_depth_buffer(swapchain_extent);

        FramebufferInfo fb_info = {};
        fb_info.width  = swapchain_extent.width;
        fb_info.height = swapchain_extent.height;
        fb_info.depth  = 1;
        fb_info.render_pass = _forward_render_pass;
        fb_info.depth_target = depth_buffer;
        fb_info.render_targets.push_back(colour_target);

        _forward_framebuffers[idx] = create_framebuffer("forward framebuffer", fb_info);
    }
}

RenderPass* VulkanRenderer::_build_forward_render_pass(const ShaderSet& shader_set)
{
    AttachmentInfo colour_attachment = {};
    colour_attachment.format = _swapchain->get_format();
    colour_attachment.load_op = LoadOp::CLEAR;
    colour_attachment.store_op = StoreOp::STORE;
    colour_attachment.final_layout = ImageLayout::PRESENT;

    AttachmentInfo depth_stencil_attachment = {};
    depth_stencil_attachment.format = Format::D24_S8;
    depth_stencil_attachment.samples = MSAASamples::MSAA_1X;
    depth_stencil_attachment.load_op = LoadOp::CLEAR;
    depth_stencil_attachment.store_op = StoreOp::STORE;
    depth_stencil_attachment.final_layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT;

    SubPassDescription subpass_info = {};
    subpass_info.colour_attachment_infos[0] = 0;
    subpass_info.depth_stencil_attachment   = 1;
    subpass_info.colour_attachment_infos_count = 1;
    subpass_info.shader_set = &shader_set;

    SubPassDependency dep_info = {};
    dep_info.src_sync = Synchronisation{ PipelineStage::PIPELINE_STAGE_BOTTOM_OF_PIPE, MemoryAccess::MEMORY_READ };
    dep_info.dst_sync = Synchronisation{ PipelineStage::PIPELINE_STAGE_COLOUR_OUTPUT,  MemoryAccess::COLOUR_ATTACHMENT_READ | MemoryAccess::COLOUR_ATTACHMENT_WRITE };

    RenderPassInfo render_pass_info = {};
    render_pass_info.attachment_infos[0] = colour_attachment;
    render_pass_info.attachment_infos[1] = depth_stencil_attachment;
    render_pass_info.attachment_infos_count = 2;
    render_pass_info.subpasses[0] = subpass_info;
    render_pass_info.subpasses_count = 1;
    render_pass_info.subpass_dependencies[0] = dep_info;
    render_pass_info.subpass_dependency_count = 1;

    auto* render_pass = create_render_pass("forward render pass", render_pass_info);
    return render_pass;
}

void VulkanRenderer::_setup_frame_datas(void)
{
    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        _frame_datas[idx].swapchain_idx = 0xdeadbeef;
        _frame_datas[idx].acquire_sem = new Semaphore(*_device_context, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        _frame_datas[idx].present_sem = new Semaphore(*_device_context, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        _frame_datas[idx].other_sem = new Semaphore(*_device_context, VK_PIPELINE_STAGE_TRANSFER_BIT);
        _frame_datas[idx].submit_fen  = new Fence(true, *_device_context);
        _frame_datas[idx].command_buffer = new VulkanCommandBuffer(_device_context->create_command_buffer(_command_pool));
        _frame_datas[idx].other_buffer = new VulkanCommandBuffer(_device_context->create_command_buffer(_command_pool));

        BufferInfo info =
        {
            .element_count = 1,
            .element_size = sizeof(LightUniformData),
            .usage = BufferUsage::UNIFORM_BUFFER
        };
        _frame_datas[idx].per_view_data.light_data_uniform_buffer = create_buffer("point lights buffer", info);

        info.element_count = 1;
        info.element_size = sizeof(CameraData);
        _frame_datas[idx].per_view_data.camera_data_uniform_buffer = create_buffer("camera buffer", info);

        DescriptorSetInfo ds_info =
        {
            .layout = _per_view_descriptor_set_layout,
            .set = 0
        };
        _frame_datas[idx].per_view_data.descriptor_set = create_descriptor_set("per view descriptor set", ds_info);

        _frame_datas[idx].per_view_data.descriptor_set->add_uniform_buffer_binding(0, _frame_datas[idx].per_view_data.camera_data_uniform_buffer);
        _frame_datas[idx].per_view_data.descriptor_set->add_uniform_buffer_binding(1, _frame_datas[idx].per_view_data.light_data_uniform_buffer);
        write_descriptor_bindings(_frame_datas[idx].per_view_data.descriptor_set);
    }
}

// TODO take into data
void VulkanRenderer::_setup_forward(void)
{
    std::string shaders_path = "shaders/forward/";
    std::vector<char> shader_code;

    _create_descriptor_set_layouts();
    _setup_frame_datas();

    Shader* vertex_shader = _load_shader("light.vert.spv", shaders_path, ShaderStage::SHADER_STAGE_VERTEX);
    Shader* fragment_shader = _load_shader("light.frag.spv", shaders_path, ShaderStage::SHADER_STAGE_FRAGMENT);
    ShaderSet* shader_set = _load_shader_set(*vertex_shader, *fragment_shader);
    _forward_render_pass = _build_forward_render_pass(*shader_set);
    _create_framebuffers();
}

void VulkanRenderer::_process_pre_render_tasks(void)
{
    std::queue<std::function<void(void)>> process;
    std::swap(_pre_render_queue, process);

    while(!process.empty())
    {
        process.front()();
        process.pop();
    }
}

void VulkanRenderer::_update_uniform_buffers(void)
{
    if(_lights_dirty)
    {
        for(auto& fr : _frame_datas)
        {
            load_buffer(*fr.per_view_data.light_data_uniform_buffer, &_light_uniform_data, sizeof(LightUniformData), 0);
        }

        _lights_dirty = false;
    }

    if(_camera_data_dirty)
    {
        for(auto& fr : _frame_datas)
        {
           load_buffer(*fr.per_view_data.camera_data_uniform_buffer, &_camera_data, sizeof(CameraData), 0);
        }

        _camera_data_dirty = false;
    }
}

std::unordered_map<RenderPass*, std::vector<DrawItem*>> VulkanRenderer::_sort_draw_items(void)
{
    std::unordered_map<RenderPass*, std::vector<DrawItem*>> by_render_pass;

    //// Sort the draw items
    //std::sort(_draw_items.begin(), _draw_items.end(), [this](DrawItem& item1, DrawItem& item2)
    //{
    //    const MaterialInfo& mat1 = this->get_material(item1.material)->get_info();
    //    const MaterialInfo& mat2 = this->get_material(item2.material)->get_info();

    //    return mat1.render_pass_h < mat2.render_pass_h;
    //});

    for(int i = 0; i < _draw_items.size(); ++i)
    {
        DrawItem& draw_item = _draw_items[i];
        Material* m = draw_item.material;
        by_render_pass[m->get_material_info().render_pass].push_back(&_draw_items[i]);
    }

    return by_render_pass;
}

void VulkanRenderer::_process_draw_items(void)
{
    FrameData& frame_res = _frame_datas[_current_frame];
    Framebuffer* fb = _forward_framebuffers[frame_res.swapchain_idx];
    CommandBuffer* cmd = frame_res.command_buffer;

    ViewportInfo vp = { 0, 0, (float)_swapchain->get_extent().width, (float)_swapchain->get_extent().height, 0.0f, 1.0f };
    ViewportInfo sc = vp;

    cmd->set_viewport({vp});
    cmd->set_scissor({sc});

    auto by_render_pass = _sort_draw_items();

    DescriptorSet* current_view_set{ nullptr };
    const DescriptorSet* current_material_set{ nullptr };

    for(auto& it : by_render_pass)
    {
        RenderPass* rp = it.first;

        PerPassData ppd;
        ppd.framebuffer = fb;
        ppd.colour_clear = { 0.3f, 0.3f, 0.3f, 1.0f };
        ppd.depth_clear.depth = 1.0f;
        ppd.depth_clear.stencil =  0;
        ppd.render_area = { vp.width, vp.height };

        rp->begin(*cmd, ppd);

        for(auto sp : rp->get_subpasses())
        {
            //SubPass* sp = _subpasses.get(sp_h);
            auto& ss = sp->get_shader_set();
            auto& pl = ss.get_pipeline_layout();

            if(frame_res.per_view_data.descriptor_set != current_view_set)
            {
                // New view, bind descriptor set
                //DescriptorSet* ds = _descriptor_pool->get_descriptor_set(frame_res.per_view_data.descriptor_set);
                cmd->bind_descriptor_sets(PipelineBindPoint::GRAPHICS, pl, { frame_res.per_view_data.descriptor_set });
                current_view_set = frame_res.per_view_data.descriptor_set;
                // TODO: Batch binding
                // TODO: Multiple views
            }

            // Draw all items
            for(auto di_p : it.second)
            {
                Material* mat = di_p->material;
                if(&mat->get_descriptor_set() != current_material_set)
                {
                    // New material, bind descriptor set
                    //DescriptorSet* ds = _descriptor_pool->get_descriptor_set(mat->get_descriptor_set());
                    current_material_set = &mat->get_descriptor_set();
                    std::vector<const DescriptorSet*> sets = { current_material_set };
                    cmd->bind_descriptor_sets(PipelineBindPoint::GRAPHICS, pl, sets);
                    // TODO: Batch binding
                }

                cmd->push_constant(pl, di_p->per_draw_data.stages, 0, di_p->per_draw_data.bytes, di_p->per_draw_data.data);

                Mesh* mesh = di_p->mesh;
                GPUBuffer* vertex_buffer = mesh->get_vertex_buffer();
                GPUBuffer* index_buffer = mesh->get_index_buffer();

                cmd->bind_vertex_buffer(*vertex_buffer);

                if(index_buffer)
                {
                    cmd->bind_index_buffer(*index_buffer);
                    cmd->draw_indexed(index_buffer->elements_count(), 1, 0, 0, 0);
                }
                else
                {
                    cmd->draw(vertex_buffer->elements_count(), 1, 0, 0);
                }
            }

            rp->next_subpass(*cmd);
        }

        rp->end(*cmd);
    }

    _draw_items.clear();
}

GPUBuffer* VulkanRenderer::create_buffer(const std::string& name, const BufferInfo& info)
{
    VkMemoryPropertyFlags memory_flags;
    if((info.usage & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE ||
       (info.usage & BufferUsage::INDEX_BUFFER)  != BufferUsage::NONE)
    {
        memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    else
    {
        memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }

    VulkanBufferInfo vk_buffer_info = _device_context->create_buffer(info, memory_flags);
    auto rend_handle = _buffers.acquire();
    VulkanBuffer* rend_buffer = _buffers.get(rend_handle);
    rend_buffer = new(rend_buffer) VulkanBuffer(name, rend_handle, info, vk_buffer_info);

#ifdef DEBUG
    _device_context->set_debug_name(name.c_str(), VK_OBJECT_TYPE_BUFFER, (uint64_t)vk_buffer_info.buffer);
#endif

    return rend_buffer;
}

DescriptorSet* VulkanRenderer::create_descriptor_set(const std::string& name, const DescriptorSetInfo& info)
{
    VkDescriptorSetLayout vk_layout = static_cast<const VulkanDescriptorSetLayout*>(info.layout)->vk_handle();
    VulkanDescriptorSetInfo set_info = _device_context->create_descriptor_set(_descriptor_pool, vk_layout);
    auto rend_handle = _descriptor_sets.acquire();
    DescriptorSet* rend_set = _descriptor_sets.get(rend_handle);
    rend_set = new(rend_set) VulkanDescriptorSet(name, info, rend_handle, set_info);
    return rend_set;
}

DescriptorSetLayout* VulkanRenderer::create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info)
{
    VkDescriptorSetLayout vk_layout = _device_context->create_descriptor_set_layout(info);
    auto rend_handle = _descriptor_set_layouts.acquire();
    DescriptorSetLayout* rend_layout = _descriptor_set_layouts.get(rend_handle);
    rend_layout = new(rend_layout) VulkanDescriptorSetLayout(name, rend_handle, vk_layout);
    return rend_layout;
}

Framebuffer* VulkanRenderer::create_framebuffer(const std::string& name, const FramebufferInfo& info)
{
    VkFramebuffer vk_framebuffer = _device_context->create_framebuffer(info);
    auto rend_handle = _framebuffers.acquire();
    Framebuffer* rend_framebuffer = _framebuffers.get(rend_handle);
    rend_framebuffer = new(rend_framebuffer) VulkanFramebuffer(name, info, rend_handle, vk_framebuffer);
    return rend_framebuffer;
}

Material* VulkanRenderer::create_material(const std::string& name, const MaterialInfo& info)
{
    std::stringstream ss;
    ss << "Material " << name << " descriptor set";

    auto* sp = info.render_pass->get_subpasses()[0];

    DescriptorSetInfo ds_info{};
    ds_info.layout = &sp->get_shader_set().get_descriptor_set_layout(DescriptorFrequency::MATERIAL);
    ds_info.set    = DescriptorFrequency::MATERIAL;

    auto* descriptor_set = static_cast<VulkanDescriptorSet*>(create_descriptor_set(ss.str(), ds_info));

    if(info.albedo_texture != nullptr)
    {
        descriptor_set->add_texture_binding((int)MaterialBindingSlot::ALBEDO, info.albedo_texture);
    }

    write_descriptor_bindings(descriptor_set);

    auto rend_handle = _materials.acquire();
    auto* material = _materials.get(rend_handle);
    material = new(material) Material(name, info, descriptor_set, rend_handle);
    return material;
}

Mesh* VulkanRenderer::create_mesh(const std::string& name, GPUBuffer* vertex_buffer, GPUBuffer* index_buffer)
{
    auto rend_handle = _meshes.acquire();
    Mesh* mesh = _meshes.get(rend_handle);
    mesh = new(mesh) Mesh(name, vertex_buffer, index_buffer, rend_handle);
    return mesh;
}

Pipeline* VulkanRenderer::create_pipeline(const std::string& name, const PipelineInfo& info)
{
    VkPipeline vk_pipeline = _device_context->create_pipeline(info);
    auto rend_handle = _pipelines.acquire();
    Pipeline* rend_pipeline = _pipelines.get(rend_handle);
    rend_pipeline = new(rend_pipeline) VulkanPipeline(name, info, rend_handle, vk_pipeline);
    return rend_pipeline;
}

PipelineLayout* VulkanRenderer::create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info)
{
    VkPipelineLayout vk_pipeline_layout = _device_context->create_pipeline_layout(info);
    auto rend_handle = _pipeline_layouts.acquire();
    VulkanPipelineLayout* rend_pipeline_layout = _pipeline_layouts.get(rend_handle);
    rend_pipeline_layout = new(rend_pipeline_layout) VulkanPipelineLayout(name, rend_handle, vk_pipeline_layout);
    return rend_pipeline_layout;
} 

RenderPass* VulkanRenderer::create_render_pass(const std::string& name, const RenderPassInfo& info)
{
    // Create render pass
    VkRenderPass vk_render_pass = _device_context->create_render_pass(info);
    auto rend_handle = _render_passes.acquire();
    VulkanRenderPass* rend_render_pass = _render_passes.get(rend_handle);
    rend_render_pass = new(rend_render_pass) VulkanRenderPass(name, info, vk_render_pass, rend_handle);

    // Create subpasses associated with this render pass
    std::vector<SubPass*> subpasses;
    for(int i = 0; i < info.subpasses_count; ++i)
    {
        std::stringstream ss;
        ss << name << " , subpass " << i;

        SubPassInfo sp_info{};
        sp_info.render_pass = rend_render_pass;
        sp_info.subpass_index = i;
        sp_info.shader_set = info.subpasses[i].shader_set;
        rend_render_pass->add_subpass(ss.str(), sp_info);

        //SubPass* subpass = create_sub_pass(ss.str(), sp_info);
        //subpasses.push_back(subpass);
    }

    return rend_render_pass;
}

Shader* VulkanRenderer::create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type)
{
    VkShaderModule vk_shader = _device_context->create_shader(code, size_bytes);
    auto rend_handle = _shaders.acquire();
    VulkanShader* rend_shader = _shaders.get(rend_handle);
    rend_shader = new(rend_shader) VulkanShader(name, size_bytes, type, rend_handle, vk_shader);
    return rend_shader;
}

ShaderSet* VulkanRenderer::create_shader_set(const std::string& name, const ShaderSetInfo& info)
{
    std::vector<DescriptorSetLayout*> layouts;
    for(int i = 0; i < info.layouts.size(); ++i)
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
    auto rend_handle = _shader_sets.acquire();
    auto* rend_shader_set = _shader_sets.get(rend_handle);

    rend_shader_set = new(rend_shader_set) ShaderSet(name, info, rend_handle, pipeline_layout);
    return rend_shader_set;
}

SubPass* VulkanRenderer::create_sub_pass(const std::string& name, const SubPassInfo& info)
{
    PipelineInfo pl_info{};

    pl_info.shaders[0] = &info.shader_set->get_shader(ShaderIndex::SHADER_INDEX_VERTEX);
    pl_info.shaders[1] = &info.shader_set->get_shader(ShaderIndex::SHADER_INDEX_FRAGMENT);
    pl_info.vertex_binding_info.index = 0;
    pl_info.vertex_binding_info.stride = 32; // TODO: Figure out how to work out vertex bindings properly

    const auto& va_infos = info.shader_set->get_vertex_attribute_infos();
    pl_info.vertex_attribute_info_count = va_infos.size();

    for(size_t i = 0; i < va_infos.size(); ++i)
    {
        pl_info.vertex_attribute_infos[i] = va_infos[i];
        pl_info.vertex_attribute_infos[i].binding = &pl_info.vertex_binding_info;
    }

    // TODO: Make this dynamic
    pl_info.colour_blending_info.blend_attachments[0].blend_enabled = true;
    pl_info.colour_blending_info.blend_attachments[0].colour_src_factor = BlendFactor::ONE;
    pl_info.colour_blending_info.blend_attachments[0].colour_dst_factor = BlendFactor::ZERO;
    pl_info.colour_blending_info.blend_attachments_count = 1;

    pl_info.dynamic_states = DynamicState::VIEWPORT | DynamicState::SCISSOR;
    pl_info.viewport_info_count = 1;
    pl_info.scissor_info_count = 1;

    pl_info.layout = &info.shader_set->get_pipeline_layout();
    pl_info.render_pass = info.render_pass;

    Pipeline* pipeline = create_pipeline(name + " pipeline", pl_info);
    auto rend_handle = _sub_passes.acquire();
    auto* sub_pass = _sub_passes.get(rend_handle);
    sub_pass = new(sub_pass) SubPass(name, info, pipeline, rend_handle);

    return sub_pass;
}

GPUTexture* VulkanRenderer::create_texture(const std::string& name, const TextureInfo& info)
{
    VulkanImageInfo vk_image_info = _device_context->create_texture(info);
    auto rend_handle = _textures.acquire();
    VulkanTexture* rend_texture = _textures.get(rend_handle);
    rend_texture = new(rend_texture) VulkanTexture(name, rend_handle, info, vk_image_info); 
    return rend_texture;
}

GPUTexture* VulkanRenderer::_register_swapchain_image(const std::string& name, const TextureInfo& info, VkImage image)
{
    VulkanImageInfo vk_image_info = _device_context->register_swapchain_image(image, vulkan_helpers::convert_format(info.format));
    auto rend_handle = _textures.acquire(); 
    VulkanTexture* rend_texture = _textures.get(rend_handle);
    rend_texture = new(rend_texture) VulkanTexture(name, rend_handle, info, vk_image_info);
    return rend_texture;
}

void VulkanRenderer::destroy_buffer(GPUBuffer* buffer)
{
    auto* vulkan_buffer = static_cast<VulkanBuffer*>(buffer);
    auto& buffer_info = vulkan_buffer->vk_buffer_info();
    auto rend_handle = vulkan_buffer->rend_handle();
    _device_context->destroy_buffer(buffer_info);
    _buffers.release(rend_handle);
}

void VulkanRenderer::destroy_descriptor_set(DescriptorSet* set) 
{
    auto* vulkan_set = static_cast<VulkanDescriptorSet*>(set);
    auto rend_handle = vulkan_set->rend_handle();
    _device_context->destroy_descriptor_set(vulkan_set->vk_set_info());
    _descriptor_sets.release(rend_handle);
}

void VulkanRenderer::destroy_descriptor_set_layout(DescriptorSetLayout* layout) 
{
    auto* vulkan_layout = static_cast<VulkanDescriptorSetLayout*>(layout);
    auto rend_handle = vulkan_layout->rend_handle();
    _device_context->destroy_descriptor_set_layout(vulkan_layout->vk_handle());
    _descriptor_set_layouts.release(rend_handle);
}

void VulkanRenderer::destroy_framebuffer(Framebuffer* framebuffer)
{
    auto* vulkan_framebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
    auto rend_handle = vulkan_framebuffer->rend_handle();
    _device_context->destroy_framebuffer(vulkan_framebuffer->vk_handle());
    _framebuffers.release(rend_handle);
}

void VulkanRenderer::destroy_material(Material* material)
{
    auto rend_handle = material->rend_handle();
    delete material;
    _materials.release(rend_handle);
}

void VulkanRenderer::destroy_mesh(Mesh* mesh)
{
    auto rend_handle = mesh->rend_handle();
    delete mesh;
    _meshes.release(rend_handle);
}

void VulkanRenderer::destroy_pipeline(Pipeline* pipeline)
{
    auto* vulkan_pipeline = static_cast<VulkanPipeline*>(pipeline);
    auto rend_handle = pipeline->rend_handle();
    _device_context->destroy_pipeline(vulkan_pipeline->vk_handle());
    _pipelines.release(rend_handle);
}

void VulkanRenderer::destroy_pipeline_layout(PipelineLayout* pipeline_layout)
{
    auto* vulkan_pipeline_layout = static_cast<VulkanPipelineLayout*>(pipeline_layout);
    auto rend_handle = pipeline_layout->rend_handle();
    _device_context->destroy_pipeline_layout(vulkan_pipeline_layout->vk_handle());
    _pipeline_layouts.release(rend_handle);
}

void VulkanRenderer::destroy_render_pass(RenderPass* render_pass)
{
    auto* vulkan_render_pass = static_cast<VulkanRenderPass*>(render_pass);
    auto rend_handle = render_pass->rend_handle();
    _device_context->destroy_render_pass(vulkan_render_pass->vk_handle());
    _render_passes.release(rend_handle);
}

void VulkanRenderer::destroy_shader(Shader* shader)
{
    auto* vulkan_shader = static_cast<VulkanShader*>(shader);
    auto rend_handle = shader->rend_handle();
    _device_context->destroy_shader(vulkan_shader->vk_handle());
    _shaders.release(rend_handle);
}

void VulkanRenderer::destroy_shader_set(ShaderSet* shader_set)
{
    auto rend_handle = shader_set->rend_handle();
    delete shader_set;
    _shader_sets.release(rend_handle);
}

void VulkanRenderer::destroy_sub_pass(SubPass* sub_pass)
{
    auto rend_handle = sub_pass->rend_handle();
    delete sub_pass;
    _sub_passes.release(rend_handle);
}

void VulkanRenderer::destroy_texture(GPUTexture* texture)
{
    auto* vulkan_texture = static_cast<VulkanTexture*>(texture);

    const auto& vk_image_info = vulkan_texture->vk_image_info();
    if(vk_image_info.is_swapchain)
    {
        // Must unregister swapchain images, cannot destroy them in the normal manner
        _unregister_swapchain_image(texture);
        return;
    }

    auto rend_handle = vulkan_texture->rend_handle();
    _device_context->destroy_texture(vulkan_texture->vk_image_info());
    _textures.release(rend_handle);
}

void VulkanRenderer::_unregister_swapchain_image(GPUTexture* texture)
{
    auto* vulkan_texture = static_cast<VulkanTexture*>(texture);
    auto rend_handle = vulkan_texture->rend_handle();
    _device_context->unregister_swapchain_image(vulkan_texture->vk_image_info());
    _textures.release(rend_handle);
}
