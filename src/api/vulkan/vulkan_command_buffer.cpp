#include "api/vulkan/vulkan_command_buffer.h"

#include "api/vulkan/vulkan_buffer.h"
#include "api/vulkan/vulkan_descriptor_set.h"
#include "api/vulkan/vulkan_framebuffer.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_pipeline.h"
#include "api/vulkan/vulkan_pipeline_layout.h"
#include "api/vulkan/vulkan_render_pass.h"
#include "api/vulkan/vulkan_texture.h"
#include "core/descriptor_set.h"
#include "core/framebuffer.h"
#include "core/gpu_buffer.h"
#include "core/gpu_texture.h"
#include "core/pipeline.h"
#include "core/rend_defs.h"
#include "core/rend_service.h"
#include "core/render_pass.h"

#include <iostream>

using namespace rend;

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer vk_handle)
    :
        _vk_handle(vk_handle)
{
}

bool VulkanCommandBuffer::recorded(void) const
{
    return _recorded;
}

VkCommandBuffer VulkanCommandBuffer::vk_handle(void) const
{
    return _vk_handle;
}

void VulkanCommandBuffer::bind_descriptor_sets(PipelineBindPoint bind_point, const PipelineLayout& pipeline_layout, const std::vector<const DescriptorSet*> descriptor_sets)
{
    //TODO: Figure out a good array size
    const int c_descriptor_set_max = 16;

    VkPipelineBindPoint vk_bind_point   = vulkan_helpers::convert_pipeline_bind_point(bind_point);
    VkPipelineLayout vk_pipeline_layout = static_cast<const VulkanPipelineLayout&>(pipeline_layout).vk_handle();

    VkDescriptorSet vk_descriptor_sets[c_descriptor_set_max];
    for(size_t i = 0; i < descriptor_sets.size(); ++i)
    {
        auto& desc_set_info = static_cast<const VulkanDescriptorSet*>(descriptor_sets[i])->vk_set_info();
        vk_descriptor_sets[i] = desc_set_info.set;
    }

    uint32_t first_set = descriptor_sets.front()->set();

    vkCmdBindDescriptorSets(_vk_handle, vk_bind_point, vk_pipeline_layout, first_set, descriptor_sets.size(), vk_descriptor_sets, 0, nullptr);

    _recorded = true;
}

void VulkanCommandBuffer::bind_pipeline(PipelineBindPoint bind_point, const Pipeline& pipeline)
{
    VkPipeline vk_pipeline = static_cast<const VulkanPipeline&>(pipeline).vk_handle();
    vkCmdBindPipeline(_vk_handle, vulkan_helpers::convert_pipeline_bind_point(bind_point), vk_pipeline);
    _recorded = true;
}

void VulkanCommandBuffer::bind_vertex_buffer(const GPUBuffer& vertex_buffer)
{
    //TODO: Update to bind more than 1 buffer
    //TODO: Handle non-0 offsets
    //TODO: Handle non-0 first binding
    VkDeviceSize offset = 0;
    auto& vertex_buffer_info = static_cast<const VulkanBuffer&>(vertex_buffer).vk_buffer_info();

    vkCmdBindVertexBuffers(_vk_handle, 0, 1, &vertex_buffer_info.buffer, &offset);

    _recorded = true;
}

void VulkanCommandBuffer::bind_index_buffer(const GPUBuffer& index_buffer)
{
    //TODO: Handle non-0 offsets
    VkDeviceSize offset = 0;
    auto& index_buffer_info = static_cast<const VulkanBuffer&>(index_buffer).vk_buffer_info();

    vkCmdBindIndexBuffer(_vk_handle, index_buffer_info.buffer, offset, VK_INDEX_TYPE_UINT32);

    _recorded = true;
}

void VulkanCommandBuffer::blit(const GPUTexture& src, const GPUTexture& dst)
{
    auto& src_image_info = static_cast<const VulkanTexture&>(src).vk_image_info();
    auto& dst_image_info = static_cast<const VulkanTexture&>(dst).vk_image_info();

    VkImageBlit b;
    b.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    b.srcSubresource.mipLevel = 1;
    b.srcSubresource.baseArrayLayer = 0;
    b.srcSubresource.layerCount = 1;
    b.srcOffsets[0] = { 0, 0, 0 };
    b.srcOffsets[1] = { (int32_t)src.width(), (int32_t)src.height(), 0 };

    b.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    b.dstSubresource.mipLevel = 1;
    b.dstSubresource.baseArrayLayer = 0;
    b.dstSubresource.layerCount = 1;
    b.dstOffsets[0] = { 0, 0, 0 };
    b.dstOffsets[1] = { (int32_t)dst.width(), (int32_t)dst.height(), 0 };

    vkCmdBlitImage(
        _vk_handle,
        src_image_info.image,
        vulkan_helpers::convert_image_layout(src.layout()),
        dst_image_info.image,
        vulkan_helpers::convert_image_layout(dst.layout()),
        1,
        &b,
        VK_FILTER_LINEAR
    );

    _recorded = true;
}

void VulkanCommandBuffer::copy(const GPUBuffer& src, const GPUBuffer& dst, const BufferBufferCopyInfo& info)
{
    VkBufferCopy copy =
    {
        .srcOffset = info.src_offset,
        .dstOffset = info.dst_offset,
        .size      = info.size_bytes
    };

    auto& src_buffer_info = static_cast<const VulkanBuffer&>(src).vk_buffer_info();
    auto& dst_buffer_info = static_cast<const VulkanBuffer&>(dst).vk_buffer_info();

    vkCmdCopyBuffer(_vk_handle, src_buffer_info.buffer, dst_buffer_info.buffer, 1, &copy);

    _recorded = true;
}

void VulkanCommandBuffer::copy(const GPUBuffer& src, const GPUTexture& dst, const BufferImageCopyInfo& info)
{
    VkBufferImageCopy copy =
    {
        .bufferOffset           = info.buffer_offset,
        .bufferRowLength        = info.buffer_width,
        .bufferImageHeight      = info.buffer_height,
        .imageSubresource       =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel       = info.mip_level,
                .baseArrayLayer = info.base_layer,
                .layerCount     = info.layer_count
            },
        .imageOffset            = { info.image_offset_x, info.image_offset_y, info.image_offset_z },
        .imageExtent            = VkExtent3D{ info.image_width, info.image_height, info.image_depth }
    };

    auto& buffer_info = static_cast<const VulkanBuffer&>(src).vk_buffer_info();
    auto& image_info  = static_cast<const VulkanTexture&>(dst).vk_image_info();

    vkCmdCopyBufferToImage(
        _vk_handle,
        buffer_info.buffer,
        image_info.image,
        vulkan_helpers::convert_image_layout(info.image_layout),
        1,
        &copy
    );
    _recorded = true;
}

void VulkanCommandBuffer::copy(const GPUTexture& src, const GPUTexture& dst, const ImageImageCopyInfo& info)
{
    auto& src_info = static_cast<const VulkanTexture&>(src).vk_image_info();
    auto& dst_info = static_cast<const VulkanTexture&>(dst).vk_image_info();
    VkImageCopy copy = vulkan_helpers::convert_image_copy(info);

    vkCmdCopyImage(
        _vk_handle,
        src_info.image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dst_info.image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copy
    );

    _recorded = true;
}

void VulkanCommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    vkCmdDraw(_vk_handle, vertex_count, instance_count, first_vertex, first_instance);

    _recorded = true;
}

void VulkanCommandBuffer::draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
    vkCmdDrawIndexed(_vk_handle, index_count, instance_count, first_index, vertex_offset, first_instance);

    _recorded = true;
}

void VulkanCommandBuffer::push_constant(const PipelineLayout& layout, ShaderStages stages, uint32_t offset, size_t size, const void* data)
{
    VkPipelineLayout vk_layout = static_cast<const VulkanPipelineLayout&>(layout).vk_handle();
    VkShaderStageFlags shader_stage_flags = vulkan_helpers::convert_shader_stages(stages);
    vkCmdPushConstants(_vk_handle, vk_layout, shader_stage_flags, offset, size, data);
}

void VulkanCommandBuffer::transition_image(GPUTexture& texture, PipelineStages src_stages, PipelineStages dst_stages, ImageLayout new_layout)
{
    transition_image(texture, texture.layout(), texture.mips(), texture.layers(), src_stages, dst_stages, new_layout);
    texture.layout(new_layout);
}

void VulkanCommandBuffer::transition_image(
    GPUTexture& texture,
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
    image_memory_barrier.image = &texture;
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

    pipeline_barrier(barrier_info);
    _recorded = true;
}

void VulkanCommandBuffer::reset(void)
{
    vkResetCommandBuffer(_vk_handle, 0);
    _recorded = false;
}

void VulkanCommandBuffer::set_viewport(const std::vector<ViewportInfo>& viewports)
{
    VkViewport vk_viewports[4];
    for(size_t i = 0; i < viewports.size(); ++i)
    {
        vk_viewports[i].x        = viewports[i].x;
        vk_viewports[i].y        = viewports[i].y;
        vk_viewports[i].width    = viewports[i].width;
        vk_viewports[i].height   = viewports[i].height;
        vk_viewports[i].minDepth = viewports[i].min_depth;
        vk_viewports[i].maxDepth = viewports[i].max_depth;
    }

    vkCmdSetViewport(_vk_handle, 0, viewports.size(), &vk_viewports[0]);

    _recorded = true;
}

void VulkanCommandBuffer::set_scissor(const std::vector<ViewportInfo>& scissors)
{
    VkRect2D vk_scissors[4];
    for(size_t i = 0; i < scissors.size(); ++i)
    {
        vk_scissors[i].offset.x      = scissors[i].x;
        vk_scissors[i].offset.y      = scissors[i].y;
        vk_scissors[i].extent.width  = scissors[i].width;
        vk_scissors[i].extent.height = scissors[i].height;
    }

    vkCmdSetScissor(_vk_handle, 0, scissors.size(), &vk_scissors[0]);

    _recorded = true;
}

void VulkanCommandBuffer::begin(void)
{
    VkCommandBufferBeginInfo info =
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .pInheritanceInfo = nullptr
    };

    vkBeginCommandBuffer(_vk_handle, &info);

    _recorded = true;
}

void VulkanCommandBuffer::end(void)
{
    vkEndCommandBuffer(_vk_handle);
    _recorded = true;
}

void VulkanCommandBuffer::begin_render_pass(const RenderPass& render_pass, const Framebuffer& framebuffer, const RenderArea render_area, const ColourClear clear_colour, const DepthStencilClear clear_depth_stencil)
{
    VkClearValue vk_clear_values[2];
    vk_clear_values[0].color        = { clear_colour.r, clear_colour.g, clear_colour.b, clear_colour.a };
    vk_clear_values[1].depthStencil = { clear_depth_stencil.depth, clear_depth_stencil.stencil };

    VkRect2D vk_render_area{};
    vk_render_area.extent.width  = render_area.w;
    vk_render_area.extent.height = render_area.h;

    // TODO Get clear values from Framebuffer per attachment
    VkRenderPassBeginInfo vk_render_pass_begin_info{};
    vk_render_pass_begin_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    vk_render_pass_begin_info.pNext           = nullptr;
    vk_render_pass_begin_info.renderPass      = static_cast<const VulkanRenderPass&>(render_pass).vk_handle();
    vk_render_pass_begin_info.framebuffer     = static_cast<const VulkanFramebuffer&>(framebuffer).vk_handle();
    vk_render_pass_begin_info.renderArea      = vk_render_area;
    vk_render_pass_begin_info.clearValueCount = 2;
    vk_render_pass_begin_info.pClearValues    = vk_clear_values;

    vkCmdBeginRenderPass(_vk_handle, &vk_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    _recorded = true;
}

void VulkanCommandBuffer::end_render_pass(void)
{
    vkCmdEndRenderPass(_vk_handle);

    _recorded = true;
}

void VulkanCommandBuffer::next_subpass(void)
{
    vkCmdNextSubpass(_vk_handle, VK_SUBPASS_CONTENTS_INLINE);

    _recorded = true;
}

void VulkanCommandBuffer::pipeline_barrier(const PipelineBarrierInfo& info)
{
    VkImageMemoryBarrier vk_image_memory_barriers[8];
    for(size_t barrier_idx{ 0 }; barrier_idx < info.image_memory_barrier_count; ++barrier_idx)
    {
        auto& image_info = static_cast<VulkanTexture*>(info.image_memory_barriers[barrier_idx].image)->vk_image_info();

        vk_image_memory_barriers[barrier_idx].sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        vk_image_memory_barriers[barrier_idx].pNext               = nullptr;
        vk_image_memory_barriers[barrier_idx].srcAccessMask       = vulkan_helpers::convert_memory_accesses(info.image_memory_barriers[barrier_idx].src_accesses);
        vk_image_memory_barriers[barrier_idx].dstAccessMask       = vulkan_helpers::convert_memory_accesses(info.image_memory_barriers[barrier_idx].dst_accesses);
        vk_image_memory_barriers[barrier_idx].oldLayout           = vulkan_helpers::convert_image_layout(info.image_memory_barriers[barrier_idx].old_layout);
        vk_image_memory_barriers[barrier_idx].newLayout           = vulkan_helpers::convert_image_layout(info.image_memory_barriers[barrier_idx].new_layout);
        vk_image_memory_barriers[barrier_idx].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_image_memory_barriers[barrier_idx].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vk_image_memory_barriers[barrier_idx].image               = image_info.image;
        vk_image_memory_barriers[barrier_idx].subresourceRange    =
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = info.image_memory_barriers[barrier_idx].base_mip_level,
            .levelCount     = info.image_memory_barriers[barrier_idx].mip_level_count,
            .baseArrayLayer = info.image_memory_barriers[barrier_idx].base_layer,
            .layerCount     = info.image_memory_barriers[barrier_idx].layers_count
        };
    }

    vkCmdPipelineBarrier(
       _vk_handle,
       vulkan_helpers::convert_pipeline_stages(info.src_stages),
       vulkan_helpers::convert_pipeline_stages(info.dst_stages),
       VK_DEPENDENCY_BY_REGION_BIT,
       0, nullptr,
       0, nullptr,
       info.image_memory_barrier_count, vk_image_memory_barriers
    );

    _recorded = true;
}
