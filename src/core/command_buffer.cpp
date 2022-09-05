#include "core/command_buffer.h"

#include "core/device_context.h"
#include "core/framebuffer.h"
#include "core/gpu_buffer.h"
#include "core/gpu_texture.h"
#include "core/pipeline.h"
#include "core/pipeline_layout.h"
#include "core/rend_defs.h"
#include "core/rend_service.h"
#include "core/render_pass.h"

#include <iostream>

using namespace rend;

CommandBuffer::CommandBuffer(void)
{
}

CommandBuffer::~CommandBuffer(void)
{
}

bool CommandBuffer::recorded(void) const
{
    return _recorded;
}

CommandBufferHandle CommandBuffer::handle(void) const
{
    return _handle;
}

void CommandBuffer::bind_descriptor_sets(PipelineBindPoint bind_point, const PipelineLayout& pipeline_layout, const std::vector<DescriptorSet*> descriptor_sets)
{
    auto& ctx = *RendService::device_context();
    ctx.bind_descriptor_sets(_handle, bind_point, pipeline_layout.handle(), descriptor_sets);
    _recorded = true;
}

void CommandBuffer::bind_pipeline(PipelineBindPoint bind_point, const Pipeline& pipeline)
{
    auto& ctx = *RendService::device_context();
    ctx.bind_pipeline(_handle, bind_point, pipeline.handle());
    _recorded = true;
}

void CommandBuffer::bind_vertex_buffer(const GPUBuffer& vertex_buffer)
{
    auto& ctx = *RendService::device_context();
    ctx.bind_vertex_buffer(_handle, vertex_buffer.handle());
    _recorded = true;
}

void CommandBuffer::bind_index_buffer(const GPUBuffer& index_buffer)
{
    auto& ctx = *RendService::device_context();
    ctx.bind_index_buffer(_handle, index_buffer.handle());
    _recorded = true;
}

void CommandBuffer::blit(const GPUTexture& src, const GPUTexture& dst)
{
    auto& ctx = *RendService::device_context();
    uint32_t off[8] =
    {
        0, 0, src.width(), dst.height(),
        0, 0, dst.width(), dst.height()
    };

    ctx.blit(
        _handle,
        src.handle(),
        dst.handle(),
        ImageLayout::TRANSFER_SRC,
        ImageLayout::TRANSFER_DST,
        off
    );
    _recorded = true;
}

void CommandBuffer::copy(const GPUBuffer& src, const GPUBuffer& dst, const BufferBufferCopyInfo& info)
{
#ifdef DEBUG
    std::cout << "copy_buffer_to_buffer(). src: " << src.name() << ". dst: " << dst.name() << std::endl;
#endif

    auto& ctx = *RendService::device_context();
    ctx.copy_buffer_to_buffer(_handle, src.handle(), dst.handle(), info);
}

void CommandBuffer::copy(const GPUBuffer& src, const GPUTexture& dst, const BufferImageCopyInfo& info)
{
    auto& ctx = *RendService::device_context();
    ctx.copy_buffer_to_image(_handle, src.handle(), dst.handle(), info);
}

void CommandBuffer::copy(const GPUTexture& src, const GPUTexture& dst, const ImageImageCopyInfo& info)
{
    auto& ctx = *RendService::device_context();
    ctx.copy_image_to_image(_handle, src.handle(), dst.handle(), info);
    _recorded = true;
}

void CommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    auto& ctx = *RendService::device_context();
    ctx.draw(_handle, vertex_count, instance_count, first_vertex, first_instance);
    _recorded = true;
}

void CommandBuffer::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
    auto& ctx = *RendService::device_context();
    ctx.draw_indexed(_handle, index_count, instance_count, first_index, vertex_offset, first_instance);
    _recorded = true;
}

void CommandBuffer::push_constant(const PipelineLayout& layout, ShaderStages stages, uint32_t offset, size_t size, const void* data)
{
    auto& ctx = *RendService::device_context();
    ctx.push_constant(_handle, layout.handle(), stages, offset, size, data);
    _recorded = true;
}

void CommandBuffer::transition_image(GPUTexture& texture, PipelineStages src_stages, PipelineStages dst_stages, ImageLayout new_layout)
{
    transition_image(texture.handle(), texture.layout(), texture.mips(), texture.layers(), src_stages, dst_stages, new_layout);
    texture.layout(new_layout);
}

void CommandBuffer::transition_image(
    TextureHandle handle,
    ImageLayout layout,
    uint32_t mips,
    uint32_t layers,
    PipelineStages src_stages,
    PipelineStages dst_stages,
    ImageLayout new_layout)
{
    ImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.old_layout = layout;
    image_memory_barrier.new_layout = new_layout;
    image_memory_barrier.image_handle = handle;
    image_memory_barrier.mip_level_count = mips;
    image_memory_barrier.layers_count = layers;

    PipelineBarrierInfo barrier_info{};
    barrier_info.src_stages = src_stages;
    barrier_info.dst_stages = dst_stages;
    barrier_info.image_memory_barriers = &image_memory_barrier;
    barrier_info.image_memory_barrier_count = 1;

    switch(layout)
    {
        case ImageLayout::UNDEFINED:
            image_memory_barrier.src_accesses = MemoryAccess::NO_ACCESS; break;
        //case VK_IMAGE_LAYOUT_PREINITIALIZED:
        //    barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT; break;
        case ImageLayout::COLOUR_ATTACHMENT:
            image_memory_barrier.src_accesses = MemoryAccess::COLOUR_ATTACHMENT_WRITE; break;
        case ImageLayout::DEPTH_STENCIL_ATTACHMENT:
        //case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            image_memory_barrier.src_accesses = MemoryAccess::DEPTH_STENCIL_ATTACHMENT_WRITE; break;
        case ImageLayout::TRANSFER_SRC:
            image_memory_barrier.src_accesses = MemoryAccess::TRANSFER_READ; break;
        case ImageLayout::TRANSFER_DST:
            image_memory_barrier.src_accesses = MemoryAccess::TRANSFER_WRITE; break;
        case ImageLayout::SHADER_READ_ONLY:
            image_memory_barrier.src_accesses = MemoryAccess::SHADER_READ; break;
        case ImageLayout::PRESENT:
            image_memory_barrier.src_accesses = MemoryAccess::MEMORY_READ; break;
        default:
            return;
    }

    switch(new_layout)
    {
        case ImageLayout::COLOUR_ATTACHMENT:
            image_memory_barrier.dst_accesses = MemoryAccess::COLOUR_ATTACHMENT_WRITE; break;
        case ImageLayout::DEPTH_STENCIL_ATTACHMENT:
            image_memory_barrier.dst_accesses = MemoryAccess::DEPTH_STENCIL_ATTACHMENT_WRITE; break;
        case ImageLayout::TRANSFER_SRC:
            image_memory_barrier.dst_accesses = MemoryAccess::TRANSFER_READ; break;
        case ImageLayout::TRANSFER_DST:
            image_memory_barrier.dst_accesses = MemoryAccess::TRANSFER_WRITE; break;
        case ImageLayout::SHADER_READ_ONLY:
            if(image_memory_barrier.src_accesses == 0)
                image_memory_barrier.src_accesses= MemoryAccess::HOST_WRITE | MemoryAccess::TRANSFER_WRITE;
            image_memory_barrier.dst_accesses = MemoryAccess::SHADER_READ; break;
        case ImageLayout::PRESENT:
        //case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
            image_memory_barrier.dst_accesses = MemoryAccess::MEMORY_READ; break;
        default:
            return;
    }

    auto& ctx = *RendService::device_context();
    ctx.pipeline_barrier(_handle, barrier_info);
    _recorded = true;
}

void CommandBuffer::reset(void)
{
    auto& ctx = *RendService::device_context();
    ctx.command_buffer_reset(_handle);
    _recorded = false;
}

void CommandBuffer::set_viewport(const ViewportInfo* viewport_infos, size_t viewport_infos_count)
{
    auto& ctx = *RendService::device_context();
    ctx.set_viewport(_handle, viewport_infos, viewport_infos_count);
    _recorded = true;
}

void CommandBuffer::set_scissor(const ViewportInfo* scissor_infos, size_t scissor_infos_count)
{
    auto& ctx = *RendService::device_context();
    ctx.set_scissor(_handle, scissor_infos, scissor_infos_count);
    _recorded = true;
}

void CommandBuffer::begin(void)
{
    auto& ctx = *RendService::device_context();
    ctx.command_buffer_begin(_handle);
    _recorded = true;
}

void CommandBuffer::end(void)
{
    auto& ctx = *RendService::device_context();
    ctx.command_buffer_end(_handle);
    _recorded = true;
}

void CommandBuffer::begin_render_pass(RenderPass& render_pass, Framebuffer& framebuffer, const RenderArea render_area, const ColourClear clear_colour, const DepthStencilClear clear_depth_stencil)
{
    auto& ctx = *RendService::device_context();
    ctx.begin_render_pass(_handle, render_pass.handle(), framebuffer.handle(), render_area, clear_colour, clear_depth_stencil);
    _recorded = true;
}

void CommandBuffer::end_render_pass(void)
{
    auto& ctx = *RendService::device_context();
    ctx.end_render_pass(_handle);
    _recorded = true;
}

void CommandBuffer::next_subpass(void)
{
    auto& ctx = *RendService::device_context();
    ctx.next_subpass(_handle);
    _recorded = true;
}
