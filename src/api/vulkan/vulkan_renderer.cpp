#include "api/vulkan/vulkan_renderer.h"

#include "core/command_buffer.h"
#include "core/command_pool.h"
#include "core/descriptor_set.h"
#include "core/descriptor_pool.h"
#include "core/device_context.h"
#include "core/rend_defs.h"
#include "core/rend_service.h"

#include "api/vulkan/fence.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/swapchain.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_semaphore.h"

#include <assert.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace rend;

VulkanRenderer::VulkanRenderer(const std::string& resource_path)
    :
        Renderer(resource_path)
{
    _swapchain = new Swapchain(3);

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

    _descriptor_pool = new DescriptorPool(pool_info);
    _command_pool = new CommandPool;

    size_t bytes = 32 * 1080 * 1080;
    BufferInfo info{ 1, bytes, BufferUsage::UNIFORM_BUFFER };
    _staging_buffers.initialise(info);
}

VulkanRenderer::~VulkanRenderer(void)
{
    static_cast<VulkanDeviceContext&>(*RendService::device_context()).get_device()->wait_idle();

    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        delete _frame_datas[idx].acquire_sem;
        delete _frame_datas[idx].present_sem;
        delete _frame_datas[idx].submit_fen;
    }

    // Delete our GPUTexture references to the swapchain
    // This needs to be done now before the swapchain is deleted, as the swapchain will destroy the VkImages
    for(auto swapchain_image_h : _swapchain->get_back_buffer_handles())
    {
        _gpu_textures.deallocate(swapchain_image_h);
    }

    delete _command_pool;
    delete _descriptor_pool;
    delete _swapchain;
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

    // Release command buffers from previous usage of this FrameData
    frame_res.submit_fen->wait();
    frame_res.command_buffer->reset();

    // Release staging buffers from previous usage of this FrameData
    for(auto staging_buffer_h : frame_res.staging_buffers_used)
    {
        _staging_buffers.release(staging_buffer_h);
    }
}

void VulkanRenderer::end_frame(void)
{
    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());

    FrameData& frame_res = _frame_datas[_current_frame];

    frame_res.command_buffer->begin();
    {
        _process_pre_render_tasks();
        _update_uniform_buffers();
        _process_draw_items();
    }
    frame_res.command_buffer->end();

    if(frame_res.command_buffer->recorded())
    {
        frame_res.submit_fen->reset();
        VkCommandBuffer vk_command_buffer = ctx.get_command_buffer(frame_res.command_buffer->handle());
        ctx.get_device()->queue_submit(&vk_command_buffer, 1, QueueType::GRAPHICS, { frame_res.acquire_sem }, { frame_res.present_sem }, frame_res.submit_fen);
        _swapchain->present(QueueType::GRAPHICS, { frame_res.present_sem });
    }
}

void VulkanRenderer::resize(void)
{
    for(auto handle : _forward_framebuffers)
    {
        Framebuffer* fb = _framebuffers.get(handle);

        for(auto rt : fb->get_colour_attachments())
        {
            _gpu_textures.deallocate(rt);
        }

        if(fb->get_depth_stencil_attachment() != NULL_HANDLE)
        {
            _gpu_textures.deallocate(fb->get_depth_stencil_attachment());
        }

        _framebuffers.deallocate(handle);
    }

    _swapchain_targets.clear();
    _forward_framebuffers.clear();

    _swapchain->recreate();
    _create_swapchain_textures();
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

void VulkanRenderer::load_texture(TextureHandle texture, const void* data, size_t bytes, uint32_t offset)
{
    transition(texture, PipelineStage::PIPELINE_STAGE_TOP_OF_PIPE, PipelineStage::PIPELINE_STAGE_TRANSFER, ImageLayout::TRANSFER_DST);

    FrameData& fr = _frame_datas[_current_frame];
    CommandBuffer* cmd = fr.command_buffer;
    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());
    void* mapped = NULL;

    BufferHandle staging_buffer_h = _staging_buffers.acquire();
    GPUBuffer* staging_buffer = _staging_buffers.get(staging_buffer_h);
    GPUTexture* image = get_texture(texture);

    mapped = ctx.map_buffer_memory(staging_buffer->handle(), staging_buffer->bytes());
    memcpy(mapped, data, bytes);
    ctx.unmap_buffer_memory(staging_buffer->handle());

    BufferImageCopyInfo info =
    {
        .buffer_offset  = 0,
        .buffer_width   = image->width(),
        .buffer_height  = image->height(),
        .image_offset_x = 0,
        .image_offset_y = 0,
        .image_offset_z = 0,
        .image_width    = image->width(),
        .image_height   = image->height(),
        .image_depth    = image->depth(),
        .image_layout   = image->layout(),
        .mip_level      = 0,
        .base_layer     = 0,
        .layer_count    = image->layers()
    };

    cmd->copy(*staging_buffer, *image, info);
    transition(texture, PipelineStage::PIPELINE_STAGE_TRANSFER, PipelineStage::PIPELINE_STAGE_FRAGMENT_SHADER, ImageLayout::SHADER_READ_ONLY);

    fr.staging_buffers_used.push_back(staging_buffer_h);
}

void VulkanRenderer::load_buffer(BufferHandle buffer_h, const void* data, size_t bytes, uint32_t offset)
{
    GPUBuffer* buffer = get_buffer(buffer_h);

    // TODO: This should be based on the memory properties, not the resource
    bool is_device_local{ false };
    if ((buffer->usage() & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE ||
        (buffer->usage() & BufferUsage::INDEX_BUFFER)  != BufferUsage::NONE)
    {
        is_device_local = true;
    }

    auto& ctx = static_cast<VulkanDeviceContext&>(*RendService::device_context());
    void* mapped = NULL;

    if(is_device_local)
    {
        BufferHandle staging_buffer_h = _staging_buffers.acquire();
        GPUBuffer* staging_buffer = _staging_buffers.get(staging_buffer_h);
        FrameData& fr = _frame_datas[_current_frame];
        CommandBuffer* cmd = fr.command_buffer;

        mapped = ctx.map_buffer_memory(staging_buffer->handle(), staging_buffer->bytes());
        memcpy(mapped, data, bytes);
        ctx.unmap_buffer_memory(staging_buffer->handle());

        BufferBufferCopyInfo info =
        {
            .size_bytes = buffer->bytes(),
            .src_offset = 0,
            .dst_offset = 0
        };

        cmd->copy(*staging_buffer, *buffer, info);

        fr.staging_buffers_used.push_back(staging_buffer_h);
    }
    else
    {
        mapped = ctx.map_buffer_memory(buffer->handle(), buffer->bytes());
        memcpy(mapped, data, bytes);
        ctx.unmap_buffer_memory(buffer->handle());
    }
}

void VulkanRenderer::transition(TextureHandle texture_h, PipelineStages src, PipelineStages dst, ImageLayout final_layout)
{
    FrameData& fr = _frame_datas[_current_frame];
    CommandBuffer* cmd = fr.command_buffer;
    GPUTexture* texture = get_texture(texture_h);
    cmd->transition_image(*texture, src, dst, final_layout);
}

Swapchain* VulkanRenderer::get_swapchain(void) const
{
    return _swapchain;
}

void VulkanRenderer::_create_swapchain_textures(void)
{
    TextureInfo backbuffer_info{};
    get_size_by_ratio(SizeRatio::FULL, backbuffer_info.width, backbuffer_info.height);
    backbuffer_info.depth = 0;
    backbuffer_info.mips = 0;
    backbuffer_info.layers = 0;
    backbuffer_info.format = _swapchain->get_format();
    backbuffer_info.layout = ImageLayout::UNDEFINED;
    backbuffer_info.samples = MSAASamples::MSAA_1X;
    backbuffer_info.usage = ImageUsage::COLOUR_ATTACHMENT | ImageUsage::TRANSFER_DST;

    auto backbuffer_handles = _swapchain->get_back_buffer_handles();
    for(size_t i = 0; i < backbuffer_handles.size(); ++i)
    {
        _swapchain_targets.push_back(_gpu_textures.allocate("backbuffer " + std::to_string(i), backbuffer_info, backbuffer_handles[i]));
    }
}

ShaderHandle VulkanRenderer::_load_shader(const std::string& name, const std::string& path, rend::ShaderStage stage)
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
        DescriptorSetLayoutInfo dsl_info{};
        DescriptorSetLayoutBinding camera_data_binding =
        {
            .binding = 0,
            .descriptor_type = DescriptorType::UNIFORM_BUFFER,
            .descriptor_count = 1,
            .shader_stages = ShaderStage::SHADER_STAGE_VERTEX
        };
        dsl_info.layout_bindings.push_back(camera_data_binding);

        DescriptorSetLayoutBinding light_data_binding =
        {
            .binding = 1,
            .descriptor_type = DescriptorType::UNIFORM_BUFFER,
            .descriptor_count = 1,
            .shader_stages = ShaderStage::SHADER_STAGE_FRAGMENT
        };
        dsl_info.layout_bindings.push_back(light_data_binding);

        _per_view_descriptor_set_layout_h = _desc_set_layouts.allocate("per view", dsl_info);
    }

    // Create and add per material descriptor set layout
    {
        DescriptorSetLayoutInfo dsl_info{};

        DescriptorSetLayoutBinding diffuse_texture_binding =
        {
            .binding = 0,
            .descriptor_type = DescriptorType::COMBINED_IMAGE_SAMPLER,
            .descriptor_count = 1,
            .shader_stages = ShaderStage::SHADER_STAGE_FRAGMENT
        };
        dsl_info.layout_bindings.push_back(diffuse_texture_binding);

        _per_material_descriptor_set_layout_h = _desc_set_layouts.allocate("per view", dsl_info);
    }
}

ShaderSetHandle VulkanRenderer::_load_shader_set(ShaderHandle vertex_shader, ShaderHandle fragment_shader)
{
    ShaderSetInfo shader_set_info{};
    shader_set_info.shaders[ShaderIndex::SHADER_INDEX_VERTEX]   = vertex_shader;
    shader_set_info.shaders[ShaderIndex::SHADER_INDEX_FRAGMENT] = fragment_shader;
    shader_set_info.vertex_attribute_infos.push_back({ .location = 0, .offset = 0 });
    shader_set_info.vertex_attribute_infos.push_back({ .location = 1, .offset = 12 });
    shader_set_info.vertex_attribute_infos.push_back({ .location = 2, .offset = 24 });
    shader_set_info.layouts[DescriptorFrequency::VIEW] = _per_view_descriptor_set_layout_h;
    shader_set_info.layouts[DescriptorFrequency::MATERIAL] = _per_material_descriptor_set_layout_h;

    PushConstantRange range =
    {
        .shader_stages = ShaderStage::SHADER_STAGE_VERTEX | ShaderStage::SHADER_STAGE_FRAGMENT,
        .offset = 0,
        .size = 68
    };
    shader_set_info.push_constant_ranges.push_back(range);

    return create_shader_set("light", shader_set_info);
}

BufferHandle VulkanRenderer::_create_depth_buffer(VkExtent2D extent)
{
    TextureInfo info{ extent.width, extent.height, 1, {}, 1, 1, Format::D24_S8, ImageLayout::UNDEFINED,  MSAASamples::MSAA_1X, ImageUsage::DEPTH_STENCIL };
    return _gpu_textures.allocate("depth buffer", info);
}

void VulkanRenderer::_create_framebuffers(void)
{
    VkExtent2D swapchain_extent = _swapchain->get_extent();

    _forward_framebuffers.clear();
    _forward_framebuffers.resize(_swapchain_targets.size());

    for(size_t idx = 0; idx < _swapchain_targets.size(); ++idx)
    {
        BufferHandle depth_buffer_handle = _create_depth_buffer(swapchain_extent);

        FramebufferInfo fb_info = {};
        fb_info.width  = swapchain_extent.width;
        fb_info.height = swapchain_extent.height;
        fb_info.depth  = 1;
        fb_info.render_pass = _forward_render_pass;
        fb_info.depth_target = depth_buffer_handle;
        fb_info.render_targets.push_back(_swapchain_targets[idx]);

        _forward_framebuffers[idx] = create_framebuffer("forward framebuffer", fb_info);
    }
}

RenderPassHandle VulkanRenderer::_build_forward_render_pass(ShaderSetHandle shader_set)
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

    SubpassInfo subpass_info = {};
    subpass_info.colour_attachment_infos[0] = 0;
    subpass_info.depth_stencil_attachment   = 1;
    subpass_info.colour_attachment_infos_count = 1;
    subpass_info.shader_set_handle = shader_set;

    SubpassDependency dep_info = {};
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

    return _render_passes.allocate("forward render pass", render_pass_info);
}

void VulkanRenderer::_setup_frame_datas(void)
{
    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        _frame_datas[idx].swapchain_idx = 0xdeadbeef;
        _frame_datas[idx].acquire_sem = new Semaphore;
        _frame_datas[idx].present_sem = new Semaphore;
        _frame_datas[idx].submit_fen  = new Fence(true);
        _frame_datas[idx].command_buffer = _command_pool->create_command_buffer();

        BufferInfo info =
        {
            .element_count = 1,
            .element_size = sizeof(LightUniformData),
            .usage = BufferUsage::UNIFORM_BUFFER
        };
        _frame_datas[idx].per_view_data.light_data_uniform_buffer_h = _gpu_buffers.allocate("point lights buffer", info);

        info.element_count = 1;
        info.element_size = sizeof(CameraData);
        _frame_datas[idx].per_view_data.camera_data_uniform_buffer_h = _gpu_buffers.allocate("camera buffer", info);

        _frame_datas[idx].per_view_data.descriptor_set = create_descriptor_set(_per_view_descriptor_set_layout_h, 0);
        DescriptorSet* ds = _descriptor_pool->get_descriptor_set(_frame_datas[idx].per_view_data.descriptor_set);
        ds->add_uniform_buffer_binding(0, _frame_datas[idx].per_view_data.camera_data_uniform_buffer_h);
        ds->add_uniform_buffer_binding(1, _frame_datas[idx].per_view_data.light_data_uniform_buffer_h);
        ds->write_bindings();
    }
}

// TODO take into data
void VulkanRenderer::_setup_forward(void)
{
    std::string shaders_path = "shaders/forward/";
    std::vector<char> shader_code;

    _create_descriptor_set_layouts();
    _setup_frame_datas();

    ShaderHandle vertex_shader = _load_shader("light.vert.spv", shaders_path, ShaderStage::SHADER_STAGE_VERTEX);
    ShaderHandle fragment_shader = _load_shader("light.frag.spv", shaders_path, ShaderStage::SHADER_STAGE_FRAGMENT);
    ShaderSetHandle shader_set = _load_shader_set(vertex_shader, fragment_shader);
    _forward_render_pass = _build_forward_render_pass(shader_set);
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
            load_buffer(fr.per_view_data.light_data_uniform_buffer_h, &_light_uniform_data, sizeof(LightUniformData), 0);
        }

        _lights_dirty = false;
    }

    if(_camera_data_dirty)
    {
        for(auto& fr : _frame_datas)
        {
           load_buffer(fr.per_view_data.camera_data_uniform_buffer_h, &_camera_data, sizeof(CameraData), 0);
        }

        _camera_data_dirty = false;
    }
}

std::unordered_map<RenderPassHandle, std::vector<DrawItem*>> VulkanRenderer::_sort_draw_items(void)
{
    std::unordered_map<RenderPassHandle, std::vector<DrawItem*>> by_render_pass;

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
        MaterialHandle m_h = draw_item.material;
        Material* m = _materials.get(m_h);

        const auto& m_info = m->get_material_info();
        by_render_pass[m_info.render_pass_h].push_back(&_draw_items[i]);
    }

    return by_render_pass;
}

void VulkanRenderer::_process_draw_items(void)
{
    FrameData& frame_res = _frame_datas[_current_frame];
    FramebufferHandle fb_handle = _forward_framebuffers[frame_res.swapchain_idx];
    CommandBuffer* cmd = frame_res.command_buffer;

    ViewportInfo vp = { 0, 0, (float)_swapchain->get_extent().width, (float)_swapchain->get_extent().height, 0.0f, 1.0f };
    ViewportInfo sc = vp;

    cmd->set_viewport(&vp, 1);
    cmd->set_scissor(&sc, 1);

    auto by_render_pass = _sort_draw_items();

    DescriptorSetHandle current_view_set_h{ NULL_HANDLE };
    DescriptorSetHandle current_material_set_h{ NULL_HANDLE };

    for(auto& it : by_render_pass)
    {
        RenderPassHandle rp_h = it.first;
        RenderPass* rp = _render_passes.get(rp_h);

        PerPassData ppd;
        ppd.framebuffer = fb_handle;
        ppd.colour_clear = { 0.3f, 0.3f, 0.3f, 1.0f };
        ppd.depth_clear.depth = 1.0f;
        ppd.depth_clear.stencil =  0;
        ppd.render_area = { vp.width, vp.height };

        rp->begin(*cmd, ppd);

        for(auto sp_h : rp->get_subpasses())
        {
            SubPass* sp = _subpasses.get(sp_h);
            ShaderSet* ss = _shader_sets.get(sp->get_shader_set());
            PipelineLayout* pl = _pipeline_layouts.get(ss->get_pipeline_layout());

            if(frame_res.per_view_data.descriptor_set != current_view_set_h)
            {
                // New view, bind descriptor set
                DescriptorSet* ds = _descriptor_pool->get_descriptor_set(frame_res.per_view_data.descriptor_set);
                cmd->bind_descriptor_sets(PipelineBindPoint::GRAPHICS, *pl, { ds });
                current_view_set_h = frame_res.per_view_data.descriptor_set;
                // TODO: Batch binding
                // TODO: Multiple views
            }

            // Draw all items
            for(auto di_p : it.second)
            {
                Material* mat = _materials.get(di_p->material);
                if(mat->get_descriptor_set() != current_material_set_h)
                {
                    // New material, bind descriptor set
                    DescriptorSet* ds = _descriptor_pool->get_descriptor_set(mat->get_descriptor_set());
                    cmd->bind_descriptor_sets(PipelineBindPoint::GRAPHICS, *pl, { ds });
                    current_material_set_h = mat->get_descriptor_set();
                    // TODO: Batch binding
                }

                cmd->push_constant(*pl, di_p->per_draw_data.stages, 0, di_p->per_draw_data.bytes, di_p->per_draw_data.data);

                Mesh* mesh = get_mesh(di_p->mesh);
                GPUBuffer* vertex_buffer = get_buffer(mesh->get_vertex_buffer());
                GPUBuffer* index_buffer = get_buffer(mesh->get_index_buffer());

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
