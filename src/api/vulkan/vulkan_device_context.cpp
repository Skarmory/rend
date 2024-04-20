#include "api/vulkan/vulkan_device_context.h"

#include "core/descriptor_set.h"
#include "core/descriptor_set_layout.h"
#include "core/framebuffer.h"
#include "core/gpu_buffer.h"
#include "core/gpu_texture.h"
#include "core/pipeline_layout.h"
#include "core/rend.h"
#include "core/render_pass.h"
#include "core/sub_pass.h"
#include "core/window.h"

#include "api/vulkan/extension_funcs.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/physical_device.h"
#include "api/vulkan/vulkan_buffer.h"
#include "api/vulkan/vulkan_descriptor_set.h"
#include "api/vulkan/vulkan_descriptor_set_layout.h"
#include "api/vulkan/vulkan_framebuffer.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_pipeline.h"
#include "api/vulkan/vulkan_pipeline_layout.h"
#include "api/vulkan/vulkan_render_pass.h"
#include "api/vulkan/vulkan_shader.h"
#include "api/vulkan/vulkan_texture.h"

#include <array>
#include <cassert>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

using namespace rend;

namespace
{
    const char* c_debug_utils_name = "VK_EXT_debug_utils";
}

VulkanDeviceContext::VulkanDeviceContext(const VulkanInitInfo& vk_init_info, const Window& window)
{
#if DEBUG
    const_cast<VulkanInitInfo&>(vk_init_info).extensions.push_back(::c_debug_utils_name);
#endif

    // Create Vulkan instance
    _vulkan_instance = new VulkanInstance(vk_init_info.extensions, vk_init_info.layers);

    // Create surface
    _vulkan_instance->create_surface(window);

    // Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    _vulkan_instance->enumerate_physical_devices(physical_devices);

    for(size_t pd_idx = 0; pd_idx < physical_devices.size(); ++pd_idx)
    {
        _physical_devices.push_back(new PhysicalDevice(*_vulkan_instance, pd_idx, physical_devices[pd_idx]));
    }

    // Choose GPU
    if((_chosen_gpu = _find_physical_device(vk_init_info.features)) == nullptr)
    {
        std::string error_string = "GPU with desired features not found";
        throw std::runtime_error(error_string);
    }

    // Create logical device
    if((_logical_device = _chosen_gpu->create_logical_device(vk_init_info.queues)) == nullptr)
    {
        std::string error_string = "Failed to create logical device";
        throw std::runtime_error(error_string);
    }
}

VulkanDeviceContext::~VulkanDeviceContext(void)
{
    delete _logical_device;

    for(auto physical_device : _physical_devices)
    {
        delete physical_device;
    }

    delete _vulkan_instance;
}

PhysicalDevice* VulkanDeviceContext::gpu(void) const
{
    return _chosen_gpu;
}

LogicalDevice* VulkanDeviceContext::get_device(void) const
{
    return _logical_device;
}

const VulkanInstance& VulkanDeviceContext::vulkan_instance(void) const
{
    return *_vulkan_instance;
}

VulkanBufferInfo VulkanDeviceContext::create_buffer(const BufferInfo& info, VkMemoryPropertyFlags memory_properties)
{
    uint32_t queue_family_index = _logical_device->get_queue_family(QueueType::GRAPHICS)->get_index();

    // Create buffer
    VkBufferCreateInfo create_info    = vulkan_helpers::gen_buffer_create_info();
    create_info.size                  = info.element_count * info.element_size;
    create_info.usage                 = vulkan_helpers::convert_buffer_usage_flags(info.usage);
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices   = &queue_family_index;
    VkBuffer buffer = _logical_device->create_buffer(create_info);

    // Allocate memory and bind
    VkMemoryRequirements memory_reqs = _logical_device->get_buffer_memory_reqs(buffer);
    VkMemoryAllocateInfo alloc_info = vulkan_helpers::gen_memory_allocate_info();
    alloc_info.allocationSize = memory_reqs.size;
    alloc_info.memoryTypeIndex = _logical_device->find_memory_type(memory_reqs.memoryTypeBits, memory_properties);
    VkDeviceMemory memory = _logical_device->allocate_memory(alloc_info);
    _logical_device->bind_buffer_memory(buffer, memory);

    // Store related data in struct and return
    VulkanBufferInfo buffer_info{};
    buffer_info.buffer = buffer;
    buffer_info.memory = memory;
    buffer_info.bytes  = memory_reqs.size;

    return buffer_info;
}

VulkanImageInfo VulkanDeviceContext::create_texture(const TextureInfo& info)
{
    // Create VkImage
    VkExtent3D vk_extent = VkExtent3D{ info.width, info.height, info.depth };
    VkImageType vk_type = vulkan_helpers::find_image_type(vk_extent);
    VkFormat vk_format = vulkan_helpers::convert_format(info.format);
    VkImageUsageFlags vk_usage = vulkan_helpers::convert_image_usage_flags(info.usage);
    VkImageAspectFlags vk_aspect = vulkan_helpers::find_image_aspects(vk_format);

    uint32_t queue_family_index = _logical_device->get_queue_family(QueueType::GRAPHICS)->get_index();
    VkImageCreateInfo create_info = vulkan_helpers::gen_image_create_info();
    create_info.format = vk_format;
    create_info.imageType = vk_type;
    create_info.extent = vk_extent;
    create_info.mipLevels = info.mips;
    create_info.arrayLayers = info.layers;
    create_info.samples = vulkan_helpers::convert_sample_count(info.samples);
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage = vk_usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices = &queue_family_index;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImage image = _logical_device->create_image(create_info);

    // Allocate memory and bind
    VkMemoryPropertyFlags mem_props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryRequirements mem_reqs = _logical_device->get_image_memory_reqs(image);
    VkMemoryAllocateInfo alloc_info = vulkan_helpers::gen_memory_allocate_info();
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = _logical_device->find_memory_type(mem_reqs.memoryTypeBits, mem_props);

    auto memory = _logical_device->allocate_memory(alloc_info);
    if (memory == VK_NULL_HANDLE)
    {
        _logical_device->destroy_image(image);
        return {};
    }
    _logical_device->bind_image_memory(image, memory);

    // Create view and sampler
    VkImageView view = _create_image_view(image, vk_format, VK_IMAGE_VIEW_TYPE_2D, vk_aspect, info.mips, info.layers);
    VkSampler sampler = _create_sampler();

    // Group related data in struct and return
    VulkanImageInfo image_info{};
    image_info.image   = image;
    image_info.memory  = memory;
    image_info.view    = view;
    image_info.sampler = sampler;

    return image_info;
}

VkShaderModule VulkanDeviceContext::create_shader(const void* code, const size_t bytes)
{
    VkShaderModuleCreateInfo info = vulkan_helpers::gen_shader_module_create_info();
    info.codeSize = bytes;
    info.pCode = static_cast<const uint32_t*>(code);

    VkShaderModule vk_module = _logical_device->create_shader_module(info);
    return vk_module;
}

VkFramebuffer VulkanDeviceContext::create_framebuffer(const FramebufferInfo& info)
{
    VkImageView vk_image_views[rend::constants::max_framebuffer_attachments];
    size_t attachment_count = info.render_targets.size();

    for(size_t idx{ 0 }; idx < info.render_targets.size(); ++idx)
    {
        //TextureHandle attachment_handle = info.render_targets[idx];
        //VulkanImageInfo& image_info = *_vk_image_infos.get(attachment_handle);
        //VkImageView vk_image_view = *_vk_image_views.get(image_info.view_handle);

        auto* vk_rt = static_cast<VulkanTexture*>(info.render_targets[idx]);
        vk_image_views[idx] = vk_rt->vk_image_info().view;
    }

    if(info.depth_target)
    {
        //VulkanImageInfo& image_info = *_vk_image_infos.get(info.depth_target);
        //VkImageView vk_image_view = *_vk_image_views.get(image_info.view_handle);

        auto* vk_dt = static_cast<VulkanTexture*>(info.depth_target);
        vk_image_views[attachment_count] = vk_dt->vk_image_info().view;
        ++attachment_count;
    }

    VkFramebufferCreateInfo create_info = vulkan_helpers::gen_framebuffer_create_info();
    create_info.renderPass      = static_cast<VulkanRenderPass*>(info.render_pass)->vk_handle();
    create_info.attachmentCount = attachment_count;
    create_info.pAttachments    = vk_image_views;
    create_info.width           = info.width;
    create_info.height          = info.height;
    create_info.layers          = info.depth;

    VkFramebuffer vk_framebuffer = _logical_device->create_framebuffer(create_info);
    return vk_framebuffer;
    //if(vk_framebuffer == VK_NULL_HANDLE)
    //{
    //    return NULL_HANDLE;
    //}

    //FramebufferHandle fb_handle = _vk_framebuffers.allocate(vk_framebuffer);
    //return fb_handle;
}

VkRenderPass VulkanDeviceContext::create_render_pass(const RenderPassInfo& info)
{
    VkSubpassDescription    vk_subpass_descs[rend::constants::max_subpasses];
    VkSubpassDependency     vk_subpass_deps[rend::constants::max_subpasses + 1];
    VkAttachmentDescription vk_attachment_descs[rend::constants::max_framebuffer_attachments];

    VkAttachmentReference vk_attachment_refs[rend::constants::max_subpasses * rend::constants::max_framebuffer_attachments];
    uint32_t              vk_preserve_refs[rend::constants::max_subpasses* rend::constants::max_framebuffer_attachments];
    size_t                vk_attachment_ref_block_start{ 0 };
    size_t                vk_preserve_ref_block_start{ 0 };

    for(uint32_t subpass_idx{ 0 }; subpass_idx < info.subpasses_count; ++subpass_idx)
    {
        const SubPassDescription& rend_subpass  = info.subpasses[subpass_idx];
        VkSubpassDescription& vk_subpass = vk_subpass_descs[subpass_idx];

        vk_subpass.flags = 0;
        vk_subpass.pipelineBindPoint = vulkan_helpers::convert_pipeline_bind_point(rend_subpass.bind_point);

        // Colour attachments
        {
            size_t colour_attach_block_start = vk_attachment_ref_block_start;

            for(size_t attachment_idx{ 0 }; attachment_idx < rend_subpass.colour_attachment_infos_count; ++attachment_idx)
            {
                VkAttachmentReference& ref = vk_attachment_refs[colour_attach_block_start + attachment_idx];
                ref.attachment = rend_subpass.colour_attachment_infos[attachment_idx];
                ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            vk_subpass.colorAttachmentCount = rend_subpass.colour_attachment_infos_count;
            vk_subpass.pColorAttachments    = &vk_attachment_refs[colour_attach_block_start];
            vk_attachment_ref_block_start   = (colour_attach_block_start + rend_subpass.colour_attachment_infos_count);
        }

        // Input attachments
        {
            size_t input_attach_block_start = vk_attachment_ref_block_start;

            for(size_t attachment_idx{ 0 }; attachment_idx < rend_subpass.input_attachment_infos_count; ++attachment_idx)
            {
                VkAttachmentReference& ref = vk_attachment_refs[input_attach_block_start + attachment_idx];
                ref.attachment = rend_subpass.input_attachment_infos[attachment_idx];
                ref.layout     = VK_IMAGE_LAYOUT_UNDEFINED;
            }

            vk_subpass.inputAttachmentCount = rend_subpass.input_attachment_infos_count;
            vk_subpass.pInputAttachments    = &vk_attachment_refs[input_attach_block_start];
            vk_attachment_ref_block_start   = (input_attach_block_start + rend_subpass.input_attachment_infos_count);
        }

        // Preserve
        {
            size_t preserve_attach_block_start = vk_preserve_ref_block_start;

            for(size_t attachment_idx{ 0 }; attachment_idx < rend_subpass.preserve_attachments_count; ++attachment_idx)
            {
                vk_preserve_refs[preserve_attach_block_start + attachment_idx] = rend_subpass.preserve_attachments[attachment_idx];
            }

            vk_subpass.preserveAttachmentCount = rend_subpass.preserve_attachments_count;
            vk_subpass.pPreserveAttachments    = &vk_preserve_refs[preserve_attach_block_start];
            vk_preserve_ref_block_start = (preserve_attach_block_start + rend_subpass.preserve_attachments_count);
        }

        // Resolve
        {
            size_t resolve_attach_block_start = vk_attachment_ref_block_start;

            for(size_t attachment_idx{ 0 }; attachment_idx < rend_subpass.resolve_attachment_infos_count; ++attachment_idx)
            {
                VkAttachmentReference& ref = vk_attachment_refs[resolve_attach_block_start + attachment_idx];
                ref.attachment = rend_subpass.resolve_attachment_infos[attachment_idx];
                ref.layout     = VK_IMAGE_LAYOUT_UNDEFINED;
            }

            if(rend_subpass.resolve_attachment_infos_count > 0)
            {
                vk_subpass.pResolveAttachments = &vk_attachment_refs[resolve_attach_block_start];
            }
            else
            {
                vk_subpass.pResolveAttachments = nullptr;
            }

            vk_attachment_ref_block_start  = (resolve_attach_block_start + rend_subpass.resolve_attachment_infos_count);
        }

        // Depth stencil
        {
            VkAttachmentReference& ref = vk_attachment_refs[vk_attachment_ref_block_start];
            ref.attachment = rend_subpass.depth_stencil_attachment;
            ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            vk_subpass.pDepthStencilAttachment = &vk_attachment_refs[vk_attachment_ref_block_start];
        }
    }

    // Set the subpass dependencies
    for(size_t subpass_dep_idx{ 0 }; subpass_dep_idx < info.subpass_dependency_count; ++subpass_dep_idx)
    {
        VkSubpassDependency& vk_subpass_dep = vk_subpass_deps[subpass_dep_idx];
        const SubPassDependency& rend_subpass_dep = info.subpass_dependencies[subpass_dep_idx];

        vk_subpass_dep.srcSubpass    = subpass_dep_idx - 1;
        vk_subpass_dep.dstSubpass    = subpass_dep_idx;
        vk_subpass_dep.srcStageMask  = vulkan_helpers::convert_pipeline_stages(rend_subpass_dep.src_sync.stages);
        vk_subpass_dep.dstStageMask  = vulkan_helpers::convert_pipeline_stages(rend_subpass_dep.dst_sync.stages);
        vk_subpass_dep.srcAccessMask = vulkan_helpers::convert_memory_accesses(rend_subpass_dep.src_sync.accesses);
        vk_subpass_dep.dstAccessMask = vulkan_helpers::convert_memory_accesses(rend_subpass_dep.dst_sync.accesses);
        vk_subpass_dep.dependencyFlags = 0;
    }

    // Set the external passes properly
    VkSubpassDependency& final_dep = vk_subpass_deps[info.subpass_dependency_count-1];
    vk_subpass_deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    vk_subpass_deps[info.subpass_dependency_count] =
    {
        info.subpass_dependency_count - 1,
        VK_SUBPASS_EXTERNAL,
        final_dep.dstStageMask,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        final_dep.dstAccessMask,
        VK_ACCESS_MEMORY_READ_BIT,
        static_cast<VkDependencyFlags>(0)
    };

    // Set the attachment descriptions
    {
        for(size_t attachment_idx{ 0 }; attachment_idx < info.attachment_infos_count; ++attachment_idx)
        {
            vk_attachment_descs[attachment_idx] = vulkan_helpers::convert_attachment_description(info.attachment_infos[attachment_idx]);
        }
    }

    VkRenderPassCreateInfo create_info = vulkan_helpers::gen_render_pass_create_info();
    create_info.attachmentCount = info.attachment_infos_count;
    create_info.pAttachments    = &vk_attachment_descs[0];
    create_info.subpassCount    = info.subpasses_count;
    create_info.pSubpasses      = &vk_subpass_descs[0];
    create_info.dependencyCount = info.subpasses_count + 1;
    create_info.pDependencies   = &vk_subpass_deps[0];

    VkRenderPass vk_render_pass = _logical_device->create_render_pass(create_info);
    return vk_render_pass;

    //if(vk_render_pass == VK_NULL_HANDLE)
    //{
    //    return NULL_HANDLE;
    //}
    //RenderPassHandle handle = _vk_render_passes.allocate(vk_render_pass);
    //return handle;
}

VkPipelineLayout VulkanDeviceContext::create_pipeline_layout(const PipelineLayoutInfo& info)
{
    std::vector<VkDescriptorSetLayout> vk_descriptor_set_layouts;
    vk_descriptor_set_layouts.reserve(info.descriptor_set_layouts.size());

    for(auto* desc_set_layout : info.descriptor_set_layouts)
    {
        auto* vulkan_desc_set = static_cast<VulkanDescriptorSetLayout*>(desc_set_layout);
        vk_descriptor_set_layouts.push_back(vulkan_desc_set->vk_handle());
    }

    std::vector<VkPushConstantRange> vk_push_constant_ranges;
    vk_push_constant_ranges.reserve(info.push_constant_ranges.size());

    for(auto& pcr : info.push_constant_ranges)
    {
        vk_push_constant_ranges.push_back(vulkan_helpers::convert_push_constant_range(pcr));
    }

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = vulkan_helpers::gen_pipeline_layout_create_info();
    pipeline_layout_create_info.setLayoutCount             = vk_descriptor_set_layouts.size();
    pipeline_layout_create_info.pSetLayouts                = vk_descriptor_set_layouts.data();
    pipeline_layout_create_info.pushConstantRangeCount     = vk_push_constant_ranges.size();
    pipeline_layout_create_info.pPushConstantRanges        = vk_push_constant_ranges.data();

    VkPipelineLayout pipeline_layout = _logical_device->create_pipeline_layout(pipeline_layout_create_info);
    return pipeline_layout;
}

VkPipeline VulkanDeviceContext::create_pipeline(const PipelineInfo& info)
{
    const int c_vertex_stage   = 0;
    const int c_fragment_stage = 1;

    VkGraphicsPipelineCreateInfo pipeline_create_info = vulkan_helpers::gen_graphics_pipeline_create_info();
    pipeline_create_info.layout             = static_cast<const VulkanPipelineLayout*>(info.layout)->vk_handle();
    pipeline_create_info.renderPass         = static_cast<VulkanRenderPass*>(info.render_pass)->vk_handle();
    pipeline_create_info.subpass            = info.subpass;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex  = 0;

    // Shader stages
    VkPipelineShaderStageCreateInfo shader_create_infos[SHADER_STAGE_COUNT];
    int create_info_idx{ 0 };

    const Shader* vertex_shader = info.shaders[c_vertex_stage];
    if(vertex_shader != nullptr)
    {
        shader_create_infos[create_info_idx]        = vulkan_helpers::gen_shader_stage_create_info();
        shader_create_infos[create_info_idx].module = static_cast<const VulkanShader*>(vertex_shader)->vk_handle();
        shader_create_infos[create_info_idx].pName  = "main";
        shader_create_infos[create_info_idx].pSpecializationInfo = nullptr;
        shader_create_infos[create_info_idx].stage  = VK_SHADER_STAGE_VERTEX_BIT;
        ++create_info_idx;
    }

    const Shader* fragment_shader = info.shaders[c_fragment_stage];
    if(fragment_shader != nullptr)
    {
        shader_create_infos[create_info_idx] = vulkan_helpers::gen_shader_stage_create_info();
        shader_create_infos[create_info_idx].module = static_cast<const VulkanShader*>(fragment_shader)->vk_handle();
        shader_create_infos[create_info_idx].pName = "main";
        shader_create_infos[create_info_idx].pSpecializationInfo = nullptr;
        shader_create_infos[create_info_idx].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ++create_info_idx;
    }

    pipeline_create_info.stageCount = create_info_idx;
    pipeline_create_info.pStages    = &shader_create_infos[0];

    // Color blend attachments
    create_info_idx = 0;
    VkPipelineColorBlendAttachmentState vk_colour_blend_attachments[constants::max_framebuffer_attachments];
    for(size_t colour_blend_idx{ 0 }; colour_blend_idx < info.colour_blending_info.blend_attachments_count; ++colour_blend_idx)
    {
        const ColourBlendAttachment* attachment = &info.colour_blending_info.blend_attachments[colour_blend_idx];
        vk_colour_blend_attachments[colour_blend_idx].blendEnable         = attachment->blend_enabled;
        vk_colour_blend_attachments[colour_blend_idx].srcColorBlendFactor = vulkan_helpers::convert_blend_factor(attachment->colour_src_factor);
        vk_colour_blend_attachments[colour_blend_idx].dstColorBlendFactor = vulkan_helpers::convert_blend_factor(attachment->colour_dst_factor);
        vk_colour_blend_attachments[colour_blend_idx].colorBlendOp        = vulkan_helpers::convert_blend_op(attachment->colour_blend_op);
        vk_colour_blend_attachments[colour_blend_idx].srcAlphaBlendFactor = vulkan_helpers::convert_blend_factor(attachment->alpha_src_factor);
        vk_colour_blend_attachments[colour_blend_idx].dstAlphaBlendFactor = vulkan_helpers::convert_blend_factor(attachment->alpha_dst_factor);
        vk_colour_blend_attachments[colour_blend_idx].alphaBlendOp        = vulkan_helpers::convert_blend_op(attachment->alpha_blend_op);
        vk_colour_blend_attachments[colour_blend_idx].colorWriteMask      = static_cast<VkColorComponentFlags>(attachment->colour_write_mask);
        ++create_info_idx;
    }

    VkPipelineColorBlendStateCreateInfo colour_blend_state_create_info = vulkan_helpers::gen_colour_blend_state_create_info();
    colour_blend_state_create_info.logicOpEnable     = info.colour_blending_info.logic_op_enabled;
    colour_blend_state_create_info.logicOp           = vulkan_helpers::convert_logic_op(info.colour_blending_info.logic_op);
    colour_blend_state_create_info.attachmentCount   = create_info_idx;
    colour_blend_state_create_info.pAttachments      = vk_colour_blend_attachments;
    colour_blend_state_create_info.blendConstants[0] = info.colour_blending_info.blend_constants[0];
    colour_blend_state_create_info.blendConstants[1] = info.colour_blending_info.blend_constants[1];
    colour_blend_state_create_info.blendConstants[2] = info.colour_blending_info.blend_constants[2];
    colour_blend_state_create_info.blendConstants[3] = info.colour_blending_info.blend_constants[3];

    pipeline_create_info.pColorBlendState = &colour_blend_state_create_info;

    // Dynamic states 
    create_info_idx = 0;
    VkDynamicState vk_dynamic_states[constants::max_dynamic_states];
    if((info.dynamic_states & DynamicState::VIEWPORT) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::VIEWPORT);
    if((info.dynamic_states & DynamicState::SCISSOR) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::SCISSOR);
    if((info.dynamic_states & DynamicState::LINE_WIDTH) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::LINE_WIDTH);
    if((info.dynamic_states & DynamicState::DEPTH_BIAS) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::DEPTH_BIAS);
    if((info.dynamic_states & DynamicState::BLEND_CONSTANTS) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::BLEND_CONSTANTS);
    if((info.dynamic_states & DynamicState::DEPTH_BOUNDS) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::DEPTH_BOUNDS);
    if((info.dynamic_states & DynamicState::STENCIL_COMPARE_MASK) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::STENCIL_COMPARE_MASK);
    if((info.dynamic_states & DynamicState::STENCIL_WRITE_MASK) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::STENCIL_WRITE_MASK);
    if((info.dynamic_states & DynamicState::STENCIL_REFERENCE) != DynamicState::NONE) vk_dynamic_states[create_info_idx++] = vulkan_helpers::convert_dynamic_state(DynamicState::STENCIL_REFERENCE);

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = vulkan_helpers::gen_dynamic_state_create_info();
    dynamic_state_create_info.dynamicStateCount = create_info_idx;
    dynamic_state_create_info.pDynamicStates    = vk_dynamic_states;

    pipeline_create_info.pDynamicState = &dynamic_state_create_info;

    // Vertex input info
    create_info_idx = 0;
    VkVertexInputBindingDescription vk_input_binding_desc;
    vk_input_binding_desc.binding = info.vertex_binding_info.index;
    vk_input_binding_desc.stride = info.vertex_binding_info.stride;
    vk_input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vk_attribute_descs[constants::max_vertex_attributes];
    for(size_t vertex_attribute_idx{ 0 }; vertex_attribute_idx < info.vertex_attribute_info_count; ++vertex_attribute_idx)
    {
        vk_attribute_descs[vertex_attribute_idx].location = info.vertex_attribute_infos[vertex_attribute_idx].location;
        vk_attribute_descs[vertex_attribute_idx].binding = info.vertex_attribute_infos[vertex_attribute_idx].binding->index;
        vk_attribute_descs[vertex_attribute_idx].format = vulkan_helpers::convert_format(info.vertex_attribute_infos[vertex_attribute_idx].format);
        vk_attribute_descs[vertex_attribute_idx].offset = info.vertex_attribute_infos[vertex_attribute_idx].offset;
        ++create_info_idx;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = vulkan_helpers::gen_vertex_input_state_create_info();
    vertex_input_state_create_info.vertexBindingDescriptionCount   = 1;
    vertex_input_state_create_info.pVertexBindingDescriptions      = &vk_input_binding_desc;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = create_info_idx;
    vertex_input_state_create_info.pVertexAttributeDescriptions    = vk_attribute_descs;

    pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;

    // Input assembly info
    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = vulkan_helpers::gen_input_assembly_state_create_info();
    input_assembly_create_info.topology               = vulkan_helpers::convert_topology(info.topology);
    input_assembly_create_info.primitiveRestartEnable = info.primitive_restart;

    pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;

    // Tessellation info
    VkPipelineTessellationStateCreateInfo tessellation_state_create_info = vulkan_helpers::gen_tessellation_state_create_info();
    tessellation_state_create_info.patchControlPoints = info.patch_control_points;

    pipeline_create_info.pTessellationState = &tessellation_state_create_info;

    // Viewport info
    VkViewport vk_viewports[rend::constants::max_viewports];
    VkRect2D vk_scissors[rend::constants::max_scissors];

    if((info.dynamic_states & DynamicState::VIEWPORT) == DynamicState::NONE)
    {
        for(size_t i{ 0 }; i < info.viewport_info_count; ++i)
        {
            vk_viewports[i].x         = info.viewport_info[i].x;
            vk_viewports[i].y         = info.viewport_info[i].y;
            vk_viewports[i].width     = info.viewport_info[i].width;
            vk_viewports[i].height    = info.viewport_info[i].height;
            vk_viewports[i].minDepth = info.viewport_info[i].min_depth;
            vk_viewports[i].maxDepth = info.viewport_info[i].max_depth;
        }
    }

    if((info.dynamic_states & DynamicState::SCISSOR) == DynamicState::NONE)
    {
        for(size_t i{ 0 }; i < info.scissor_info_count; ++i)
        {
            vk_scissors[i].offset.x         = info.scissor_info[i].x;
            vk_scissors[i].offset.y         = info.scissor_info[i].y;
            vk_scissors[i].extent.width     = info.scissor_info[i].width;
            vk_scissors[i].extent.height    = info.scissor_info[i].height;
        }
    }

    VkPipelineViewportStateCreateInfo viewport_state_create_info = vulkan_helpers::gen_viewport_state_create_info();
    viewport_state_create_info.viewportCount = info.viewport_info_count;
    viewport_state_create_info.pViewports    = &vk_viewports[0];
    viewport_state_create_info.scissorCount  = info.scissor_info_count;
    viewport_state_create_info.pScissors     = &vk_scissors[0];

    pipeline_create_info.pViewportState = &viewport_state_create_info;

    // Rasterisation info
    VkPipelineRasterizationStateCreateInfo rasterisation_state_create_info = vulkan_helpers::gen_rasterisation_state_create_info();
    rasterisation_state_create_info.depthClampEnable        = static_cast<VkBool32>(info.rasteriser_info.depth_clamp_enabled);
    rasterisation_state_create_info.rasterizerDiscardEnable = static_cast<VkBool32>(info.rasteriser_info.discard_enabled);
    rasterisation_state_create_info.polygonMode             = vulkan_helpers::convert_polygon_mode(info.rasteriser_info.polygon_mode);
    rasterisation_state_create_info.cullMode                = vulkan_helpers::convert_cull_mode(info.rasteriser_info.cull_mode);
    rasterisation_state_create_info.frontFace               = vulkan_helpers::convert_front_face(info.rasteriser_info.front_face);
    rasterisation_state_create_info.depthBiasEnable         = static_cast<VkBool32>(info.rasteriser_info.depth_bias_enabled);
    rasterisation_state_create_info.depthBiasConstantFactor = info.rasteriser_info.depth_bias_constant_factor;
    rasterisation_state_create_info.depthBiasClamp          = info.rasteriser_info.depth_bias_clamp;
    rasterisation_state_create_info.depthBiasSlopeFactor    = info.rasteriser_info.depth_bias_slope_factor;
    rasterisation_state_create_info.lineWidth               = info.rasteriser_info.line_width;

    pipeline_create_info.pRasterizationState = &rasterisation_state_create_info;

    // Multisampling info
    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = vulkan_helpers::gen_multisample_state_create_info();
    multisample_state_create_info.rasterizationSamples  = vulkan_helpers::convert_sample_count(info.multisampling_info.sample_count);
    multisample_state_create_info.sampleShadingEnable   = info.multisampling_info.sample_shading_enabled;
    multisample_state_create_info.minSampleShading      = info.multisampling_info.min_sample_shading;
    multisample_state_create_info.pSampleMask           = &info.multisampling_info.sample_mask;
    multisample_state_create_info.alphaToCoverageEnable = info.multisampling_info.alpha_to_coverage_enabled;
    multisample_state_create_info.alphaToOneEnable      = info.multisampling_info.alpha_to_one_enabled;

    pipeline_create_info.pMultisampleState = &multisample_state_create_info;

    // Depth stencil info
    VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = vulkan_helpers::gen_depth_stencil_state_create_info();
    depth_stencil_create_info.depthTestEnable       = static_cast<VkBool32>(info.depth_stencil_info.depth_test_enabled);
    depth_stencil_create_info.depthWriteEnable      = static_cast<VkBool32>(info.depth_stencil_info.depth_write_enabled);
    depth_stencil_create_info.depthCompareOp        = vulkan_helpers::convert_compare_op(info.depth_stencil_info.compare_op);
    depth_stencil_create_info.depthBoundsTestEnable = static_cast<VkBool32>(info.depth_stencil_info.depth_bounds_test_enabled);
    depth_stencil_create_info.stencilTestEnable     = static_cast<VkBool32>(info.depth_stencil_info.stencil_test_enabled);
    depth_stencil_create_info.front.failOp          = vulkan_helpers::convert_stencil_op(info.depth_stencil_info.front_stencil_fail_op);
    depth_stencil_create_info.front.passOp          = vulkan_helpers::convert_stencil_op(info.depth_stencil_info.front_stencil_success_op);
    depth_stencil_create_info.front.depthFailOp     = vulkan_helpers::convert_stencil_op(info.depth_stencil_info.front_stencil_depth_fail_op);
    depth_stencil_create_info.front.compareOp       = vulkan_helpers::convert_compare_op(info.depth_stencil_info.front_stencil_compare_op);
    depth_stencil_create_info.front.compareMask     = info.depth_stencil_info.front_stencil_compare_mask;
    depth_stencil_create_info.front.writeMask       = info.depth_stencil_info.front_stencil_write_mask;
    depth_stencil_create_info.front.reference       = info.depth_stencil_info.front_stencil_reference;
    depth_stencil_create_info.back.failOp           = vulkan_helpers::convert_stencil_op(info.depth_stencil_info.back_stencil_fail_op);
    depth_stencil_create_info.back.passOp           = vulkan_helpers::convert_stencil_op(info.depth_stencil_info.back_stencil_success_op);
    depth_stencil_create_info.back.depthFailOp      = vulkan_helpers::convert_stencil_op(info.depth_stencil_info.back_stencil_depth_fail_op);
    depth_stencil_create_info.back.compareOp        = vulkan_helpers::convert_compare_op(info.depth_stencil_info.back_stencil_compare_op);
    depth_stencil_create_info.back.compareMask      = info.depth_stencil_info.back_stencil_compare_mask;
    depth_stencil_create_info.back.writeMask        = info.depth_stencil_info.back_stencil_write_mask;
    depth_stencil_create_info.back.reference        = info.depth_stencil_info.back_stencil_reference;
    depth_stencil_create_info.minDepthBounds        = info.depth_stencil_info.min_depth_bound;
    depth_stencil_create_info.maxDepthBounds        = info.depth_stencil_info.max_depth_bound;

    pipeline_create_info.pDepthStencilState           = &depth_stencil_create_info;

    VkPipeline pipeline = _logical_device->create_pipeline(pipeline_create_info);
    return pipeline;
}

VkDescriptorPool VulkanDeviceContext::create_descriptor_pool(const DescriptorPoolInfo& info)
{
    VkDescriptorPoolSize vk_pool_sizes[c_descriptor_types_count];
    for(size_t idx{ 0 }; idx < info.pool_sizes_count; ++idx)
    {
        vk_pool_sizes[idx].type            = vulkan_helpers::convert_descriptor_type(info.pool_sizes[idx].type);
        vk_pool_sizes[idx].descriptorCount = info.pool_sizes[idx].count;
    }

    VkDescriptorPoolCreateInfo vk_info = vulkan_helpers::gen_descriptor_pool_create_info();
    vk_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    vk_info.maxSets                    = info.max_sets;
    vk_info.pPoolSizes                 = vk_pool_sizes;
    vk_info.poolSizeCount              = info.pool_sizes_count;

   VkDescriptorPool vk_pool = _logical_device->create_descriptor_pool(vk_info); 
   return vk_pool;
}

VulkanDescriptorSetInfo VulkanDeviceContext::create_descriptor_set(VkDescriptorPool pool, VkDescriptorSetLayout layout)
{
    std::vector<VkDescriptorSetLayout> layouts = { layout };
    VkDescriptorSet vk_set = _logical_device->allocate_descriptor_sets(layouts, pool)[0];

    VulkanDescriptorSetInfo descriptor_set{};
    descriptor_set.set = vk_set;
    descriptor_set.pool = pool;
    descriptor_set.layout = layout;

    return descriptor_set;
}

VkDescriptorSetLayout VulkanDeviceContext::create_descriptor_set_layout(const DescriptorSetLayoutInfo& info)
{
    std::vector<VkDescriptorSetLayoutBinding> vk_descriptor_set_layout_bindings;

    for(size_t idx{0}; idx < info.layout_bindings.size(); ++idx)
    {
        VkDescriptorSetLayoutBinding vk_descriptor_set_layout_binding{};
        vk_descriptor_set_layout_binding.binding            = info.layout_bindings[idx].binding;
        vk_descriptor_set_layout_binding.descriptorType     = vulkan_helpers::convert_descriptor_type(info.layout_bindings[idx].descriptor_type);
        vk_descriptor_set_layout_binding.descriptorCount    = info.layout_bindings[idx].descriptor_count;
        vk_descriptor_set_layout_binding.stageFlags         = info.layout_bindings[idx].shader_stages;
        vk_descriptor_set_layout_binding.pImmutableSamplers = nullptr;

        vk_descriptor_set_layout_bindings.push_back(vk_descriptor_set_layout_binding);
    }

    VkDescriptorSetLayoutCreateInfo create_info = vulkan_helpers::gen_descriptor_set_layout_create_info();
    create_info.pBindings    = vk_descriptor_set_layout_bindings.data();
    create_info.bindingCount = vk_descriptor_set_layout_bindings.size();

    VkDescriptorSetLayout vk_descriptor_set_layout = _logical_device->create_descriptor_set_layout(create_info);
    return vk_descriptor_set_layout;
}

VkCommandBuffer VulkanDeviceContext::create_command_buffer(VkCommandPool command_pool)
{
    auto vk_command_buffers = _logical_device->allocate_command_buffers(1, VK_COMMAND_BUFFER_LEVEL_PRIMARY, command_pool);
    return vk_command_buffers[0];
}

void VulkanDeviceContext::destroy_command_buffer(VkCommandBuffer buffer, VkCommandPool pool)
{
    std::vector<VkCommandBuffer> vk_command_buffers{ buffer };
    _logical_device->free_command_buffers(vk_command_buffers, pool);
}

void VulkanDeviceContext::destroy_descriptor_pool(VkDescriptorPool pool)
{
    _logical_device->destroy_descriptor_pool(pool);
}

//TODO: Update to handle multiple
void VulkanDeviceContext::destroy_descriptor_set(const VulkanDescriptorSetInfo& set_info)
{
    _logical_device->free_descriptor_sets(&set_info.set, 1, set_info.pool);
}

void VulkanDeviceContext::destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout)
{
    _logical_device->destroy_descriptor_set_layout(descriptor_set_layout);
}

VulkanImageInfo VulkanDeviceContext::register_swapchain_image(VkImage swapchain_image, VkFormat format)
{
    VkImageView view = _create_image_view(swapchain_image, format, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);

    VulkanImageInfo image_info{};
    image_info.image = swapchain_image;
    image_info.view = view;
    image_info.is_swapchain = true;

    return image_info;
}

void VulkanDeviceContext::destroy_buffer(const VulkanBufferInfo& buffer_info)
{
    _logical_device->destroy_buffer(buffer_info.buffer);
    _logical_device->free_memory(buffer_info.memory);
}

void VulkanDeviceContext::destroy_texture(const VulkanImageInfo& image_info)
{
    if(image_info.is_swapchain)
    {
        // Only swapchain can destroy the swapchain images
        return;
    }

    if(image_info.sampler != VK_NULL_HANDLE)
    {
        _logical_device->destroy_sampler(image_info.sampler);
    }

    _logical_device->destroy_image_view(image_info.view);
    _logical_device->destroy_image(image_info.image);
    _logical_device->free_memory(image_info.memory);
}

void VulkanDeviceContext::destroy_shader(VkShaderModule shader)
{
    _logical_device->destroy_shader_module(shader);
}

void VulkanDeviceContext::destroy_framebuffer(VkFramebuffer framebuffer)
{
    _logical_device->destroy_framebuffer(framebuffer);
}

void VulkanDeviceContext::destroy_render_pass(VkRenderPass render_pass)
{
    _logical_device->destroy_render_pass(render_pass);
}

void VulkanDeviceContext::destroy_pipeline_layout(VkPipelineLayout pipeline_layout)
{
    _logical_device->destroy_pipeline_layout(pipeline_layout);
}

void VulkanDeviceContext::destroy_pipeline(VkPipeline pipeline)
{
    _logical_device->destroy_pipeline(pipeline);
}

void VulkanDeviceContext::unregister_swapchain_image(const VulkanImageInfo& image_info)
{
    _logical_device->destroy_image_view(image_info.view);
}

VkEvent VulkanDeviceContext::create_event(const VkEventCreateInfo& info)
{
    return _logical_device->create_event(info);
}

VkFence VulkanDeviceContext::create_fence(const VkFenceCreateInfo& info)
{
    return _logical_device->create_fence(info);
}

VkSemaphore VulkanDeviceContext::create_semaphore(const VkSemaphoreCreateInfo& info)
{
    return _logical_device->create_semaphore(info);
}

VkCommandPool VulkanDeviceContext::create_command_pool(void)
{
    VkCommandPoolCreateInfo create_info = vulkan_helpers::gen_command_pool_create_info();
    create_info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex        = _logical_device->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkCommandPool vk_command_pool = _logical_device->create_command_pool(create_info);
    return vk_command_pool;
}

void VulkanDeviceContext::destroy_event(VkEvent event)
{
    _logical_device->destroy_event(event);
}

void VulkanDeviceContext::destroy_fence(VkFence fence)
{
    _logical_device->destroy_fence(fence);
}

void VulkanDeviceContext::destroy_semaphore(VkSemaphore semaphore)
{
    _logical_device->destroy_semaphore(semaphore);
}

void VulkanDeviceContext::destroy_command_pool(VkCommandPool command_pool)
{
    _logical_device->destroy_command_pool(command_pool);
}

void VulkanDeviceContext::write_descriptor_bindings(VkDescriptorSet vk_set, const std::vector<DescriptorSetBinding>& bindings)
{
    std::vector<VkWriteDescriptorSet> vk_write_sets;
    std::array<VkDescriptorBufferInfo, 8> vk_desc_buffer_infos;
    std::array<VkDescriptorImageInfo, 8> vk_desc_image_infos;

    size_t desc_buffer_idx = 0;
    size_t desc_image_idx = 0;
    for(auto& binding : bindings)
    {
        VkWriteDescriptorSet write_desc = {};
        write_desc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_desc.pNext            = nullptr;
        write_desc.dstSet           = vk_set;
        write_desc.dstBinding       = binding.slot;
        write_desc.dstArrayElement  = 0;
        write_desc.descriptorCount  = 1;
        write_desc.descriptorType   = vulkan_helpers::convert_descriptor_type(binding.type);

        switch(binding.type)
        {
            case DescriptorType::COMBINED_IMAGE_SAMPLER:
            case DescriptorType::SAMPLED_IMAGE:
            {
                //VulkanImageInfo& image_info = *_vk_image_infos.get(binding.handle);

                auto& image_info = static_cast<VulkanTexture*>(binding.resource)->vk_image_info();

                VkDescriptorImageInfo vk_descriptor_image_info{};
                //vk_descriptor_image_info.sampler = *_vk_samplers.get(image_info.sampler_handle);
                //vk_descriptor_image_info.imageView = *_vk_image_views.get(image_info.view_handle);
                vk_descriptor_image_info.sampler = image_info.sampler;
                vk_descriptor_image_info.imageView = image_info.view;
                vk_descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                vk_desc_image_infos[desc_image_idx] = vk_descriptor_image_info;

                write_desc.pImageInfo = &vk_desc_image_infos[desc_image_idx];
                ++desc_image_idx;

                break;
            }

            case DescriptorType::UNIFORM_BUFFER:
            {
                //VulkanBufferInfo& buffer_info = *_vk_buffer_infos.get(binding.handle);

                auto& buffer_info = static_cast<VulkanBuffer*>(binding.resource)->vk_buffer_info();

                VkDescriptorBufferInfo vk_buffer_info{};
                vk_buffer_info.buffer  = buffer_info.buffer;
                vk_buffer_info.offset = 0;
                vk_buffer_info.range   = buffer_info.bytes;

                vk_desc_buffer_infos[desc_buffer_idx] = vk_buffer_info;

                write_desc.pBufferInfo = &vk_desc_buffer_infos[desc_buffer_idx];
                ++desc_buffer_idx;

                break;
            }
        }

        vk_write_sets.push_back(write_desc);
    }

    _logical_device->update_descriptor_sets( vk_write_sets );
}

void* VulkanDeviceContext::map_buffer_memory(GPUBuffer& buffer, size_t bytes)
{
    void* mapped = nullptr;
    auto& buffer_info = static_cast<VulkanBuffer&>(buffer).vk_buffer_info();
    _logical_device->map_memory(buffer_info.memory, bytes, 0, &mapped);
    return mapped;
}

void VulkanDeviceContext::unmap_buffer_memory(GPUBuffer& buffer)
{
    auto& buffer_info = static_cast<VulkanBuffer&>(buffer).vk_buffer_info();
    _logical_device->unmap_memory(buffer_info.memory);
}

void* VulkanDeviceContext::map_image_memory(GPUTexture& texture, size_t bytes)
{
    void* mapped = nullptr;
    auto& image_info = static_cast<VulkanTexture&>(texture).vk_image_info();
    _logical_device->map_memory(image_info.memory, bytes, 0, &mapped);
    return mapped;
}

void VulkanDeviceContext::unmap_image_memory(GPUTexture& texture)
{
    auto& image_info = static_cast<VulkanTexture&>(texture).vk_image_info();
    _logical_device->unmap_memory(image_info.memory);
}

void VulkanDeviceContext::set_debug_name(const std::string& name, VkObjectType type, uint64_t handle)
{
    VkDebugUtilsObjectNameInfoEXT info;
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.pNext = nullptr;
    info.objectType = type;
    info.objectHandle = handle;
    info.pObjectName = name.c_str();

    pfnSetDebugUtilsObjectNameEXT(_logical_device->get_handle(), &info);
}

PhysicalDevice* VulkanDeviceContext::_find_physical_device(const VkPhysicalDeviceFeatures& features)
{
    for(PhysicalDevice* device : _physical_devices)
    {
        if(device->has_features(features))
        {
            return device;
        }
    }

    return nullptr;
}

VkImageView VulkanDeviceContext::_create_image_view(VkImage image, VkFormat format, VkImageViewType type, VkImageAspectFlags aspect, uint32_t mips, uint32_t layers)
{
    VkImageViewCreateInfo create_info = vulkan_helpers::gen_image_view_create_info();
    create_info.image = image;
    create_info.viewType = type;
    create_info.format = format;
    create_info.subresourceRange.aspectMask = aspect;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = mips;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = layers;

    auto image_view = _logical_device->create_image_view(create_info);

    return image_view;
}

VkSampler VulkanDeviceContext::_create_sampler(void)
{
    VkSamplerCreateInfo create_info = vulkan_helpers::gen_sampler_create_info();
    create_info.magFilter               = VK_FILTER_LINEAR;
    create_info.minFilter               = VK_FILTER_LINEAR;
    create_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    create_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.mipLodBias              = 1.0f;
    create_info.anisotropyEnable        = VK_FALSE;
    create_info.maxAnisotropy           = 1.0f;
    create_info.compareEnable           = VK_FALSE;
    create_info.compareOp               = VK_COMPARE_OP_ALWAYS;
    create_info.minLod                  = 1.0f;
    create_info.maxLod                  = 1.0f;
    create_info.borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    create_info.unnormalizedCoordinates = VK_FALSE;

    auto sampler = _logical_device->create_sampler(create_info);

    return sampler;
}
