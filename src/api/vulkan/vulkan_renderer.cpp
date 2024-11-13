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

#include "core/logging/log_defs.h"
#include "core/logging/log_manager.h"

#include "api/vulkan/extensions.h"
#include "api/vulkan/fence.h"
#include "api/vulkan/layers.h"
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
{
    auto& logger = core::logging::LogManager::get_instance();
    logger.add_log_file(core::logging::C_RENDERER_LOG_FILE_NAME);
    logger.add_log_channel(core::logging::C_RENDERER_LOG_CHANNEL_NAME);
    logger.bind_file_to_channel(core::logging::C_RENDERER_LOG_FILE_NAME, core::logging::C_RENDERER_LOG_CHANNEL_NAME);

    auto* vk_init_info = static_cast<VulkanInitInfo*>(init_info.api_init_info);

    // Add required features
    vk_init_info->features.push_back(DeviceFeature::IMAGELESS_FRAMEBUFFER);

    // Add required extensions
#if DEBUG
    vk_init_info->extensions.push_back(vk::instance_ext::debug_utils);
#endif

    // Add required layers
#if DEBUG
    vk_init_info->layers.push_back(vk::layer::khronos::validation);
#endif

    _window = new Window(init_info.resolution_width, init_info.resolution_height, init_info.app_name);
    glfwSetInputMode(_window->get_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(_window->get_handle(), ::glfw_window_resize_callback);
    glfwSetErrorCallback(::glfw_error_callback);

    _device_context = new VulkanDeviceContext(*vk_init_info, *_window);
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

        // TODO Improve buffer creation to avoid this
        std::vector<VulkanBuffer*> buffers;
        for(size_t i = 0; i < _staging_buffers.capacity(); ++i)
        {
            buffers.push_back(_create_staging_buffer());
        }

        for(size_t i = 0; i < _staging_buffers.capacity(); ++i)
        {
            _staging_buffers.release(buffers[i]->_rend_handle);
        }
    }
}

VulkanRenderer::~VulkanRenderer(void)
{
    _device_context->get_device()->wait_idle();

    for(auto& buffer : _staging_buffers)
    {
        _destroy_staging_buffer(&buffer);
    }

    for(auto& buffer : _buffers)
    {
        destroy_buffer(&buffer);
    }

    for(auto& texture : _textures)
    {
        destroy_texture(&texture);
    }

    for(auto& shader : _shaders)
    {
        destroy_shader(&shader);
    }

    for(auto& framebuffer : _framebuffers)
    {
        destroy_framebuffer(&framebuffer);
    }

    for(auto& pipeline : _pipelines)
    {
        destroy_pipeline(&pipeline);
    }

    for(auto& pipeline_layout : _pipeline_layouts)
    {
        destroy_pipeline_layout(&pipeline_layout);
    }

    for(auto& render_pass : _render_passes)
    {
        destroy_render_pass(&render_pass);
    }

    for(auto& descriptor_set : _descriptor_sets)
    {
        destroy_descriptor_set(&descriptor_set);
    }

    for(auto& descriptor_set_layout : _descriptor_set_layouts)
    {
        destroy_descriptor_set_layout(&descriptor_set_layout);
    }

    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        delete _frame_datas[idx].submit_fen;
        delete _frame_datas[idx].load_sem;
    }

    _device_context->destroy_command_pool(_command_pool);
    _device_context->destroy_descriptor_pool(_descriptor_pool);

    delete _swapchain;
    delete _device_context;
    delete _window;
}

void VulkanRenderer::configure(void)
{
    // Setup resources for each frame in flight
    for(uint32_t idx = 0; idx < _FRAMES_IN_FLIGHT; ++idx)
    {
        const std::string name_prefix = "frame " + std::to_string(idx);
        std::string name{};

        name = name_prefix + " load semaphore";
        _frame_datas[idx].load_sem = new Semaphore(name, *_device_context, VK_PIPELINE_STAGE_TRANSFER_BIT);

        name = name_prefix + " submit fence";
        _frame_datas[idx].submit_fen = new Fence(name, true, *_device_context);

        name = name_prefix + " draw command buffer";
        _frame_datas[idx].draw_cmd = new VulkanCommandBuffer(name, _device_context->create_command_buffer(_command_pool));
#if DEBUG
        _device_context->set_debug_name(name, VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)static_cast<VulkanCommandBuffer*>(_frame_datas[idx].draw_cmd)->vk_handle());
#endif

        name = name_prefix + " load command buffer";
        _frame_datas[idx].load_cmd = new VulkanCommandBuffer(name, _device_context->create_command_buffer(_command_pool));
#if DEBUG
        _device_context->set_debug_name(name, VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)static_cast<VulkanCommandBuffer*>(_frame_datas[idx].load_cmd)->vk_handle());
#endif
    }
}

void VulkanRenderer::start_frame(void)
{
    // Update frame and grab next
    _current_frame = ++_frame_counter % _FRAMES_IN_FLIGHT;
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "RENDERER | Starting frame: " + std::to_string(_frame_counter));

    auto& frame_res = _frame_datas[_current_frame];
    frame_res.frame = _frame_counter;

    // Release staging buffers from previous usage of this FrameData
    for(auto* staging_buffer : frame_res.staging_buffers_used)
    {
        _staging_buffers.release(staging_buffer->rend_handle());
    }

    frame_res.staging_buffers_used.clear();
}

void VulkanRenderer::end_frame(void)
{
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "RENDERER | Ending frame: " + std::to_string(_frame_counter));
    FrameData& frame_res = _frame_datas[_current_frame];

    // Wait for previous submit of this frame to be complete
    frame_res.submit_fen->wait();

    if(_need_resize)
    {
        _resize();
    }

    // Acquire next swapchain image
    StatusCode code = StatusCode::SUCCESS;
    while((code = _swapchain->acquire(&frame_res.swapchain_acquisition)) != StatusCode::SUCCESS)
    {
        if(code == StatusCode::SWAPCHAIN_ACQUIRE_ERROR)
        {
            core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "RENDERER | Failed to acquire swapchain image. Killing process.");
            std::abort();
        }

        if(code == StatusCode::SWAPCHAIN_OUT_OF_DATE)
        {
            _resize();
        }
    }

    std::vector<Semaphore*> draw_wait_sems;
    draw_wait_sems.push_back(frame_res.swapchain_acquisition->acquire_semaphore);

    if(!_pre_render_queue.empty())
    {
        auto* load_cmd = static_cast<VulkanCommandBuffer*>(frame_res.load_cmd);
        load_cmd->reset();
        load_cmd->begin();
        _process_pre_render_tasks();
        load_cmd->end();

        _device_context->queue_submit(*load_cmd, QueueType::GRAPHICS, {}, { frame_res.load_sem }, nullptr);

        draw_wait_sems.push_back(frame_res.load_sem);
    }

    auto* draw_cmd = static_cast<VulkanCommandBuffer*>(frame_res.draw_cmd);
    draw_cmd->reset();
    draw_cmd->begin();
    _process_draw_items();
    draw_cmd->end();

    frame_res.submit_fen->reset(); 

    _device_context->queue_submit(
        *draw_cmd,
        QueueType::GRAPHICS,
        draw_wait_sems,
        { frame_res.swapchain_acquisition->present_semaphore },
        frame_res.submit_fen
    );

    _swapchain->present(
        *frame_res.swapchain_acquisition,
        QueueType::GRAPHICS
    );
}

void VulkanRenderer::resize(void)
{
    _need_resize = true;
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

void VulkanRenderer::load_texture(GPUTexture& texture)
{
    _pre_render_queue.push(
        [this, &texture]()
        {
            FrameData& fr = _frame_datas[_current_frame];
            VulkanCommandBuffer* cmd = static_cast<VulkanCommandBuffer*>(fr.load_cmd);
            void* mapped = NULL;

            transition(texture, PipelineStage::PIPELINE_STAGE_TOP_OF_PIPE, PipelineStage::PIPELINE_STAGE_TRANSFER, ImageLayout::TRANSFER_DST);

            auto  staging_buffer_h = _staging_buffers.acquire();
            VulkanBuffer* staging_buffer = _staging_buffers.get(staging_buffer_h);

            mapped = _device_context->map_buffer_memory(*staging_buffer, staging_buffer->bytes());
            memcpy(mapped, texture.data(), texture.bytes());
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
        });
}

void VulkanRenderer::load_buffer(GPUBuffer& buffer)
{
    _pre_render_queue.push(
        [this, &buffer]()
        {
            // TODO: This should be based on the memory properties, not the resource
            bool is_device_local{ false };
            if ((buffer.usage() & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE ||
                (buffer.usage() & BufferUsage::INDEX_BUFFER)  != BufferUsage::NONE)
            {
                is_device_local = true;
            }

            if(is_device_local)
            {
                FrameData& fr = _frame_datas[_current_frame];
                auto staging_buffer_h = _staging_buffers.acquire();
                GPUBuffer* staging_buffer = _staging_buffers.get(staging_buffer_h);
                CommandBuffer* cmd = fr.load_cmd;

                void* mapped = _device_context->map_buffer_memory(*staging_buffer, staging_buffer->bytes());
                memcpy(mapped, buffer.data(), buffer.bytes());
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
                void* mapped = _device_context->map_buffer_memory(buffer, buffer.bytes());
                memcpy(mapped, buffer.data(), buffer.bytes());
                _device_context->unmap_buffer_memory(buffer);
            }
        });
}

void VulkanRenderer::transition(GPUTexture& texture, PipelineStages src, PipelineStages dst, ImageLayout final_layout)
{
    FrameData& fr = _frame_datas[_current_frame];
    VulkanCommandBuffer* vk_cmd = static_cast<VulkanCommandBuffer*>(fr.load_cmd);
    vk_cmd->transition_image(texture, src, dst, final_layout);
}

void VulkanRenderer::write_descriptor_bindings(const DescriptorSet& descriptor_set)
{
    auto& vk_set = static_cast<const VulkanDescriptorSet&>(descriptor_set);
    auto& vk_set_info = vk_set.vk_set_info();
    _device_context->write_descriptor_bindings(vk_set_info.set, vk_set.get_bindings());
}

GPUBuffer* VulkanRenderer::get_buffer(const std::string& name) const
{
    for(const GPUBuffer& buffer : _buffers)
    {
        if(buffer.name() == name)
        {
            return const_cast<GPUBuffer*>(&buffer);
        }
    }

    return nullptr;
}

DescriptorSetLayout* VulkanRenderer::get_descriptor_set_layout(const std::string& name) const
{
    for(const DescriptorSetLayout& layout : _descriptor_set_layouts)
    {
        if(layout.name() == name)
        {
            return const_cast<DescriptorSetLayout*>(&layout);
        }
    }

    return nullptr;
}

Pipeline* VulkanRenderer::get_pipeline(const std::string& name) const
{
    for(const Pipeline& pipeline : _pipelines)
    {
        if(pipeline.name() == name)
        {
            return const_cast<Pipeline*>(&pipeline);
        }
    }

    return nullptr;
}

Swapchain* VulkanRenderer::get_swapchain(void) const
{
    return _swapchain;
}

RenderPass* VulkanRenderer::get_render_pass(const std::string& name) const
{
    for(const RenderPass& render_pass : _render_passes)
    {
        if(render_pass.name() == name)
        {
            return const_cast<RenderPass*>(&render_pass);
        }
    }

    return nullptr;
}

RenderStrategy* VulkanRenderer::get_render_strategy(const std::string& name) const
{
    for(const RenderStrategy& render_strategy : _render_strategies)
    {
        if(render_strategy.name() == name)
        {
            return const_cast<RenderStrategy*>(&render_strategy);
        }
    }

    return nullptr;
}

GPUTexture* VulkanRenderer::get_texture(const std::string& name) const
{
    for(const GPUTexture& texture : _textures)
    {
        if(texture.name() == name)
        {
            return const_cast<GPUTexture*>(&texture);
        }
    }

    return nullptr;
}

VulkanDeviceContext* VulkanRenderer::device_context(void) const
{
    return _device_context;
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

std::unordered_map<View*, std::unordered_map<RenderStrategy*, std::vector<DrawItem*>>> VulkanRenderer::_sort_draw_items(void)
{
    std::unordered_map<View*, 
        std::unordered_map<
            RenderStrategy*, std::vector<DrawItem*>>> sorted_items;

    //// Sort the draw items
    //std::sort(_draw_items.begin(), _draw_items.end(), [this](DrawItem& item1, DrawItem& item2)
    //{
    //    const MaterialInfo& mat1 = this->get_material(item1.material)->get_info();
    //    const MaterialInfo& mat2 = this->get_material(item2.material)->get_info();

    //    return mat1.render_pass_h < mat2.render_pass_h;
    //});

    for(size_t i = 0; i < _draw_items.size(); ++i)
    {
        DrawItem& draw_item = _draw_items[i];
        Material* m = draw_item.material;
        View* v = draw_item.view;

        sorted_items[v][m->get_material_info().render_strategy].push_back(&_draw_items[i]);
    }

    return sorted_items;
}

void VulkanRenderer::_process_draw_items(void)
{
    FrameData& frame_res = _frame_datas[_current_frame];
    CommandBuffer* cmd = frame_res.draw_cmd;

    ViewportInfo vp = { 0, 0, (float)_swapchain->get_extent().width, (float)_swapchain->get_extent().height, 0.0f, 1.0f };
    ViewportInfo sc = vp;
    RenderArea ra = { static_cast<uint32_t>(vp.width), static_cast<uint32_t>(vp.height) };

    cmd->set_viewport({vp});
    cmd->set_scissor({sc});

    auto sorted_items = _sort_draw_items();

    DescriptorSet* current_view_set{ nullptr };
    DescriptorSet* current_material_set{ nullptr };

    std::vector<const DescriptorSet*> to_bind;
    to_bind.reserve(2);

    for(auto& view_it : sorted_items)
    {
        auto* view = view_it.first;

        if(auto* view_descriptor_set = &view->get_descriptor_set(); view_descriptor_set != current_view_set)
        {
            current_view_set = view_descriptor_set;
            to_bind.push_back(current_view_set);
        }

        for(auto& render_strategy_it : view_it.second)
        {
            auto* render_strategy = render_strategy_it.first;

            for(auto& draw_pass : render_strategy->get_draw_passes())
            {
                auto* fb = frame_res.find_framebuffer(draw_pass.get_named_framebuffer());
                PerPassData ppd = _create_per_pass_data(*fb, draw_pass.get_colour_clear(), draw_pass.get_depth_stencil_clear(), ra);

                draw_pass.begin(*cmd, ppd);

                for(auto& sp : draw_pass.get_subpasses())
                {
                    auto& ss = sp.get_pipeline().get_shader_set();
                    auto& pl = ss.get_pipeline_layout();

                    // Draw all items
                    for(auto di_p : render_strategy_it.second)
                    {
                        Material* mat = di_p->material;
                        if(&mat->get_descriptor_set() != current_material_set)
                        {
                            // New material, bind descriptor set
                            current_material_set = &mat->get_descriptor_set();
                            to_bind.push_back(current_material_set);
                        }

                        if(to_bind.size() > 0)
                        {
                            cmd->bind_descriptor_sets(PipelineBindPoint::GRAPHICS, pl, to_bind);
                            to_bind.clear();
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

                    draw_pass.next_subpass(*cmd);
                }

                draw_pass.end(*cmd);
            }
        }
    }

    _draw_items.clear();
}

GPUBuffer* VulkanRenderer::create_buffer(const std::string& name, const BufferInfo& info)
{
    VkMemoryPropertyFlags memory_flags;
    if((info.usage & BufferUsage::VERTEX_BUFFER) != BufferUsage::NONE || (info.usage & BufferUsage::INDEX_BUFFER)  != BufferUsage::NONE)
    {
        memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    else
    {
        memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }

    VulkanBufferInfo vk_buffer_info = _device_context->create_buffer(info, memory_flags);
    auto rend_handle = _buffers.allocate(name, info, vk_buffer_info);
    auto* rend_buffer = _buffers.get(rend_handle);
    static_cast<RendObject*>(rend_buffer)->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name("Buffer: " + name, VK_OBJECT_TYPE_BUFFER, (uint64_t)vk_buffer_info.buffer);
#endif

    return rend_buffer;
}

DescriptorSet* VulkanRenderer::create_descriptor_set(const std::string& name, const DescriptorSetLayout& layout)
{
    auto vk_layout = static_cast<const VulkanDescriptorSetLayout&>(layout).vk_handle();
    auto set_info = _device_context->create_descriptor_set(_descriptor_pool, vk_layout);
    auto rend_handle = _descriptor_sets.allocate(name, layout, set_info);
    auto* rend_set = _descriptor_sets.get(rend_handle);
    rend_set->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name(rend_set->name(), VK_OBJECT_TYPE_DESCRIPTOR_SET, (uint64_t)set_info.set);
#endif

    return rend_set;
}

DescriptorSetLayout* VulkanRenderer::create_descriptor_set_layout(const std::string& name, const DescriptorSetLayoutInfo& info)
{
    auto vk_layout = _device_context->create_descriptor_set_layout(info);
    auto rend_handle = _descriptor_set_layouts.allocate(name, vk_layout, info);
    auto* rend_layout = _descriptor_set_layouts.get(rend_handle);
    rend_layout->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name("Descriptor Set Layout: " + name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t)vk_layout);
#endif

    return rend_layout;
}

void VulkanRenderer::create_framebuffer(const std::string& name, const FramebufferInfo& info)
{
    FramebufferInfo copy_info = info;

    if(info.use_size_ratio)
    {
        get_size_by_ratio(info.size_ratio, copy_info.width, copy_info.height);
        copy_info.depth = 1;
    }

    //if(!info.named_depth_target.empty())
    //{
    //    auto* depth_target = get_texture(info.named_depth_target);
    //    copy_info.depth_target = &depth_target->get_info();
    //}

    for(size_t fidx = 0; fidx < _frame_datas.size(); ++fidx)
    {
        copy_info.render_targets.clear();

        for(size_t rtidx = 0; rtidx < copy_info.named_render_targets.size(); ++rtidx)
        {
            auto& rt_name = copy_info.named_render_targets[rtidx];
            const bool is_swapchain_attachment = rt_name == C_BACKBUFFER_NAME;

            auto& rt_texture_info = is_swapchain_attachment
                ? _swapchain->get_backbuffer_texture_info() // Doesn't matter which we get -- we just need the image info
                : _frame_datas[fidx].find_render_target(copy_info.named_render_targets[rtidx])->get_info();

            copy_info.render_targets.push_back(rt_texture_info);
        }

        auto* framebuffer = _create_framebuffer(name, copy_info);
        _frame_datas[fidx].framebuffers.push_back(framebuffer);
    }
}

Pipeline* VulkanRenderer::create_pipeline(const std::string& name, const PipelineInfo& info)
{
    auto vk_pipeline = _device_context->create_pipeline(info);
    auto rend_handle = _pipelines.allocate(name, info, vk_pipeline);
    auto* rend_pipeline = _pipelines.get(rend_handle);
    rend_pipeline->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name("Pipeline: " + name, VK_OBJECT_TYPE_PIPELINE, (uint64_t)vk_pipeline);
#endif

    return rend_pipeline;
}

PipelineLayout* VulkanRenderer::create_pipeline_layout(const std::string& name, const PipelineLayoutInfo& info)
{
    auto vk_pipeline_layout = _device_context->create_pipeline_layout(info);
    auto rend_handle = _pipeline_layouts.allocate(name, vk_pipeline_layout);
    auto* rend_pipeline_layout = _pipeline_layouts.get(rend_handle);
    rend_pipeline_layout->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name("Pipeline Layout: " + name, VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)vk_pipeline_layout);
#endif

    return rend_pipeline_layout;
} 

RenderPass* VulkanRenderer::create_render_pass(const std::string& name, const RenderPassInfo& info)
{
    RenderPassInfo copy_info = info;

    for(size_t attachment_idx = 0; attachment_idx < info.attachment_infos.size(); ++attachment_idx)
    {
        auto& attachment_info = copy_info.attachment_infos[attachment_idx];

        if(attachment_info.format == rend::Format::SWAPCHAIN)
        {
            attachment_info.format = _swapchain->get_format();
        }
    }

    // Create render pass
    auto vk_render_pass = _device_context->create_render_pass(copy_info);
    auto rend_handle = _render_passes.allocate(name, copy_info, vk_render_pass);
    auto* rend_render_pass = _render_passes.get(rend_handle);
    rend_render_pass->_rend_handle = rend_handle;

    // Create subpasses associated with this render pass
    //std::vector<SubPass*> subpasses;
    //for(int i = 0; i < info.subpasses_count; ++i)
    //{
    //    std::stringstream ss;
    //    ss << name << " , Subpass " << i;

    //    SubPassInfo sp_info{};
    //    sp_info.render_pass = rend_render_pass;
    //    sp_info.subpass_index = i;
    //    sp_info.shader_set = info.subpasses[i].shader_set;
    //    SubPass*
    //    rend_render_pass->add_subpass(ss.str(), sp_info);
    //}

#ifdef DEBUG
    _device_context->set_debug_name("Render Pass: " + name, VK_OBJECT_TYPE_RENDER_PASS, (uint64_t)vk_render_pass);
#endif

    return rend_render_pass;
}

void VulkanRenderer::create_render_target(const std::string& name, const TextureInfo& info)
{
    if(info.format == rend::Format::SWAPCHAIN)
    {
        // Swapchain images already created
        return;
    }

    for(size_t i = 0; i < _frame_datas.size(); ++i)
    {
        auto& frame = _frame_datas[i];
        auto* render_target = create_texture(name, info);
        frame.render_targets.push_back(render_target);
    }
}

Shader* VulkanRenderer::create_shader(const std::string& name, const void* code, uint32_t size_bytes, ShaderStage type)
{
    auto vk_shader = _device_context->create_shader(code, size_bytes);
    auto rend_handle = _shaders.allocate(name, size_bytes, type, vk_shader);
    auto* rend_shader = _shaders.get(rend_handle);
    rend_shader->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name("Shader: " + name, VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t)vk_shader);
#endif

    return rend_shader;
}

GPUTexture* VulkanRenderer::create_texture(const std::string& name, const TextureInfo& info)
{
    RendHandle rend_handle;

    VulkanImageInfo vk_image_info;

    if(info.use_size_ratio)
    {
        TextureInfo newinfo = info;
        get_size_by_ratio(info.size_ratio, newinfo.width, newinfo.height);
        newinfo.depth = 1;
        vk_image_info = _device_context->create_texture(newinfo);
        rend_handle = _textures.allocate(name, newinfo, vk_image_info);
    }
    else
    {
        vk_image_info = _device_context->create_texture(info);
        rend_handle = _textures.allocate(name, info, vk_image_info);
    }

    auto* rend_texture = _textures.get(rend_handle);
    rend_texture->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name("Texture: " + name, VK_OBJECT_TYPE_IMAGE, (uint64_t)vk_image_info.image);
#endif

    return rend_texture;
}

Framebuffer* VulkanRenderer::_create_framebuffer(const std::string& name, const FramebufferInfo& info)
{
    auto vk_framebuffer = _device_context->create_framebuffer(info);
    auto rend_handle = _framebuffers.allocate(name, info, vk_framebuffer);
    auto* rend_framebuffer = _framebuffers.get(rend_handle);
    rend_framebuffer->_rend_handle = rend_handle;

#ifdef DEBUG
    _device_context->set_debug_name("framebuffer: " + name, VK_OBJECT_TYPE_FRAMEBUFFER, (uint64_t)vk_framebuffer);
#endif

    return rend_framebuffer;
}

PerPassData VulkanRenderer::_create_per_pass_data(const Framebuffer& fb, const ColourClear& colour_clear, const DepthStencilClear& depth_clear, const RenderArea& render_area)
{
    auto& frame_data = _frame_datas[_current_frame];

    PerPassData ppd =
    {
        .framebuffer = &fb,
        .attachments = {},
        .attachments_count = 0,
        .colour_clear = colour_clear,
        .depth_clear = depth_clear,
        .render_area = render_area
    };

    //auto& colour_attachments = fb.get_colour_attachment_names();
    //auto& depth_attachment = fb.get_depth_stencil_attachment_name();
    auto& attachment_names = fb.get_attachment_names();

    for(; ppd.attachments_count < attachment_names.size(); ++ppd.attachments_count)
    {
        auto& attachment_name = attachment_names[ppd.attachments_count];

        auto* attachment = attachment_name == Renderer::C_BACKBUFFER_NAME
            ? &_swapchain->get_backbuffer(frame_data.swapchain_acquisition->image_idx)
            : _frame_datas[_current_frame].find_render_target(attachment_name);

        ppd.attachments[ppd.attachments_count] = attachment;
    }

    //if(depth_attachment.empty())
    //{
    //    ppd.attachments[ppd.attachments_count++] = get_texture(depth_attachment);
    //}

    return ppd;
}

VulkanBuffer* VulkanRenderer::_create_staging_buffer(void)
{
    static int _staging_buffers_creation_count = 0;
    const size_t STAGING_BUFFER_BYTES = 32 * 1080 * 1080;
    static const BufferInfo STAGING_BUFFER_INFO{ 1, STAGING_BUFFER_BYTES, BufferUsage::TRANSFER_SRC };

    VulkanBufferInfo vk_info = _device_context->create_buffer(STAGING_BUFFER_INFO, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    auto buffer_handle = _staging_buffers.acquire();
    auto* buffer = _staging_buffers.get(buffer_handle);
    buffer = new(buffer) VulkanBuffer("staging buffer " + std::to_string(++_staging_buffers_creation_count), STAGING_BUFFER_INFO, vk_info);
    buffer->_rend_handle = buffer_handle;

    return buffer;
}

void VulkanRenderer::_destroy_staging_buffer(VulkanBuffer* buffer)
{
    auto& vk_buffer_info = buffer->vk_buffer_info();
    _device_context->destroy_buffer(vk_buffer_info);

    auto handle = buffer->_rend_handle;
    _staging_buffers.release(handle);
}

void VulkanRenderer::destroy_buffer(GPUBuffer* buffer)
{
    auto* vulkan_buffer = static_cast<VulkanBuffer*>(buffer);
    auto& buffer_info = vulkan_buffer->vk_buffer_info();
    auto rend_handle = vulkan_buffer->rend_handle();
    _device_context->destroy_buffer(buffer_info);
    _buffers.deallocate(rend_handle);
}

void VulkanRenderer::destroy_descriptor_set(DescriptorSet* set) 
{
    auto* vulkan_set = static_cast<VulkanDescriptorSet*>(set);
    auto rend_handle = vulkan_set->rend_handle();
    _device_context->destroy_descriptor_set(vulkan_set->vk_set_info());
    _descriptor_sets.deallocate(rend_handle);
}

void VulkanRenderer::destroy_descriptor_set_layout(DescriptorSetLayout* layout) 
{
    auto* vulkan_layout = static_cast<VulkanDescriptorSetLayout*>(layout);
    auto rend_handle = vulkan_layout->rend_handle();
    _device_context->destroy_descriptor_set_layout(vulkan_layout->vk_handle());
    _descriptor_set_layouts.deallocate(rend_handle);
}

void VulkanRenderer::destroy_framebuffer(Framebuffer* framebuffer)
{
    auto* vulkan_framebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
    auto rend_handle = vulkan_framebuffer->rend_handle();
    _device_context->destroy_framebuffer(vulkan_framebuffer->vk_handle());
    _framebuffers.deallocate(rend_handle);
}

void VulkanRenderer::destroy_pipeline(Pipeline* pipeline)
{
    auto* vulkan_pipeline = static_cast<VulkanPipeline*>(pipeline);
    auto rend_handle = pipeline->rend_handle();
    _device_context->destroy_pipeline(vulkan_pipeline->vk_handle());
    _pipelines.deallocate(rend_handle);
}

void VulkanRenderer::destroy_pipeline_layout(PipelineLayout* pipeline_layout)
{
    auto* vulkan_pipeline_layout = static_cast<VulkanPipelineLayout*>(pipeline_layout);
    auto rend_handle = pipeline_layout->rend_handle();
    _device_context->destroy_pipeline_layout(vulkan_pipeline_layout->vk_handle());
    _pipeline_layouts.deallocate(rend_handle);
}

void VulkanRenderer::destroy_render_pass(RenderPass* render_pass)
{
    auto* vulkan_render_pass = static_cast<VulkanRenderPass*>(render_pass);
    auto rend_handle = render_pass->rend_handle();
    _device_context->destroy_render_pass(vulkan_render_pass->vk_handle());
    _render_passes.deallocate(rend_handle);
}

void VulkanRenderer::destroy_shader(Shader* shader)
{
    auto* vulkan_shader = static_cast<VulkanShader*>(shader);
    auto rend_handle = shader->rend_handle();
    _device_context->destroy_shader(vulkan_shader->vk_handle());
    _shaders.deallocate(rend_handle);
}

void VulkanRenderer::destroy_texture(GPUTexture* texture)
{
    auto* vulkan_texture = static_cast<VulkanTexture*>(texture);

    const auto& vk_image_info = vulkan_texture->vk_image_info();
    if(vk_image_info.is_swapchain)
    {
        // Swapchain manages backbuffer textures
        return;
    }

    auto rend_handle = vulkan_texture->rend_handle();
    _device_context->destroy_texture(vk_image_info);
    _textures.deallocate(rend_handle);
}

void VulkanRenderer::_resize(void)
{
#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "RENDERER | Resize");
#endif

    _device_context->get_device()->wait_idle();
    _swapchain->recreate();

    // Register new swapchain images
    //std::vector<VulkanTexture*> backbuffer_textures;
    //auto& backbuffer_images = _swapchain->get_back_buffer_textures();
    //for(size_t idx = 0; idx < backbuffer_images.size(); ++idx)
    //{
    //    backbuffer_textures.push_back(_register_swapchain_image("backbuffer " + idx, _backbuffer_textures[0]->get_info(), backbuffer_images[idx]));
    //}

    // Swap newly registered swapchain textures with old ones
    //_backbuffer_textures.swap(backbuffer_textures);

    // Unregister all the old swapchain textures
    //for(auto* backbuffer_texture : backbuffer_textures)
    //{
    //    _unregister_swapchain_image(backbuffer_texture);
    //}

    // Resize the frame resources
    for(uint32_t fidx = 0; fidx < _FRAMES_IN_FLIGHT; ++fidx)
    {
        auto& frame = _frame_datas[fidx];
        std::vector<GPUTexture*> render_targets;
        std::vector<Framebuffer*> framebuffers;
        render_targets.reserve(frame.render_targets.size());
        framebuffers.reserve(frame.framebuffers.size());

        for(size_t rtidx = 0; rtidx < frame.render_targets.size(); ++rtidx)
        {
            std::string name = frame.render_targets[rtidx]->name();
            TextureInfo texture_info = frame.render_targets[rtidx]->get_info();
            destroy_texture(frame.render_targets[rtidx]);
            render_targets.push_back(create_texture(name, texture_info));
        }

        for(size_t fbidx = 0; fbidx < frame.framebuffers.size(); ++fbidx)
        {
            std::string name = frame.framebuffers[fbidx]->name();
            FramebufferInfo fb_info = frame.framebuffers[fbidx]->get_info();
            destroy_framebuffer(frame.framebuffers[fbidx]);
            get_size_by_ratio(fb_info.size_ratio, fb_info.width, fb_info.height);
            framebuffers.push_back(_create_framebuffer(name, fb_info));
        }

        frame.render_targets.swap(render_targets);
        frame.framebuffers.swap(framebuffers);
    }

    _need_resize = false;
}
