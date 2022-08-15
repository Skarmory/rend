#include "api/vulkan/vulkan_device_context.h"

#include "core/command_pool.h"
#include "core/descriptor_set.h"
#include "core/gpu_buffer.h"
#include "core/gpu_texture.h"
#include "core/rend.h"
#include "core/rend_service.h"
#include "core/window.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/physical_device.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_instance.h"

#include <cassert>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace rend;

VulkanDeviceContext::VulkanDeviceContext( const RendInitInfo& rend_info )
{
    VulkanInitInfo* vk_init_info = static_cast<VulkanInitInfo*>(rend_info.api_init_info);

    // Create Vulkan instance
    _vulkan_instance = new VulkanInstance(vk_init_info->extensions, vk_init_info->extensions_count, vk_init_info->layers, vk_init_info->layers_count);

    // Create surface
    _vulkan_instance->create_surface();

    // Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    _vulkan_instance->enumerate_physical_devices(physical_devices);

    for(size_t physical_device_index = 0; physical_device_index < physical_devices.size(); physical_device_index++)
    {
        PhysicalDevice* pdev = new PhysicalDevice(*_vulkan_instance, physical_device_index, physical_devices[physical_device_index]);
        _physical_devices.push_back(pdev);
    }

    // Choose GPU
    if((_chosen_gpu = _find_physical_device(vk_init_info->features)) == nullptr)
    {
        std::string error_string = "GPU with desired features not found";
        throw std::runtime_error(error_string);
    }

    // Create logical device
    if((_logical_device = _chosen_gpu->create_logical_device(vk_init_info->queues)) == nullptr)
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

VertexBufferHandle VulkanDeviceContext::create_vertex_buffer(uint32_t vertices_count, size_t vertex_size)
{
    return static_cast<IndexBufferHandle>(_create_buffer_internal(
        vertices_count * vertex_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    ));
}

IndexBufferHandle VulkanDeviceContext::create_index_buffer(uint32_t indices_count, size_t index_size)
{
    return static_cast<IndexBufferHandle>(_create_buffer_internal(
        indices_count * index_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    ));
}

UniformBufferHandle VulkanDeviceContext::create_uniform_buffer(size_t bytes)
{
    return static_cast<UniformBufferHandle>(_create_buffer_internal(
        bytes,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    ));
}

TextureHandle VulkanDeviceContext::create_texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t mips, uint32_t layers, Format format, MSAASamples samples, ImageUsage usage)
{
    VkExtent3D vk_extent = VkExtent3D{ width, height, depth };
    VkImageType vk_type = vulkan_helpers::find_image_type(vk_extent);
    VkFormat vk_format = vulkan_helpers::convert_format(format);
    VkImageUsageFlags vk_usage = vulkan_helpers::convert_image_usage_flags(usage);
    VkImageAspectFlags vk_aspect = vulkan_helpers::find_image_aspects(vk_format);

    uint32_t queue_family_index = _logical_device->get_queue_family(QueueType::GRAPHICS)->get_index();
    VkImageCreateInfo create_info = vulkan_helpers::gen_image_create_info();
    create_info.format = vk_format;
    create_info.imageType = vk_type;
    create_info.extent = vk_extent;
    create_info.mipLevels = mips;
    create_info.arrayLayers = layers;
    create_info.samples = vulkan_helpers::convert_sample_count(samples);
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage = vk_usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices = &queue_family_index;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image = _logical_device->create_image(create_info);
    if (image == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    VkMemoryPropertyFlags mem_props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryRequirements mem_reqs = _logical_device->get_image_memory_reqs(image);
    VkMemoryAllocateInfo alloc_info = vulkan_helpers::gen_memory_allocate_info();
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = _logical_device->find_memory_type(mem_reqs.memoryTypeBits, mem_props);

    auto memory = _logical_device->allocate_memory(alloc_info);
    if (memory == VK_NULL_HANDLE)
    {
        _logical_device->destroy_image(image);
        return NULL_HANDLE;
    }

    _logical_device->bind_image_memory(image, memory);

    VkImageView view = _create_image_view(image, vk_format, VK_IMAGE_VIEW_TYPE_2D, vk_aspect, mips, layers);
    VkSampler sampler = _create_sampler();

    VulkanImageInfo image_info{};
    image_info.image = image;
    image_info.memory_handle = _vk_memorys.allocate(memory);
    image_info.view_handle = _vk_image_views.allocate(view);
    image_info.sampler_handle = _vk_samplers.allocate(sampler);

    auto handle = _vk_image_infos.allocate(image_info);

    return handle;
}

ShaderHandle VulkanDeviceContext::create_shader(const ShaderStage type, const void* code, const size_t bytes)
{
    UU(type);

    VkShaderModuleCreateInfo info = vulkan_helpers::gen_shader_module_create_info();
    info.codeSize = bytes;
    info.pCode = static_cast<const uint32_t*>(code);

    VkShaderModule vk_module = _logical_device->create_shader_module(info);
    if(vk_module == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    ShaderHandle handle = _vk_shaders.allocate(vk_module);

    return handle;
}

FramebufferHandle VulkanDeviceContext::create_framebuffer(const FramebufferInfo& info)
{
    VkRenderPass vk_render_pass = *_vk_render_passes.get(info.render_pass_handle);
    VkImageView vk_image_views[rend::constants::max_framebuffer_attachments];
    size_t attachment_count = info.render_target_handles_count;

    for(size_t idx{ 0 }; idx < info.render_target_handles_count; ++idx)
    {
        TextureHandle attachment_handle = info.render_target_handles[idx];
        VulkanImageInfo& image_info = *_vk_image_infos.get(attachment_handle);
        VkImageView vk_image_view = *_vk_image_views.get(image_info.view_handle);

        vk_image_views[idx] = vk_image_view;
    }

    if(info.depth_buffer_handle != NULL_HANDLE)
    {
        VulkanImageInfo& image_info = *_vk_image_infos.get(info.depth_buffer_handle);
        VkImageView vk_image_view = *_vk_image_views.get(image_info.view_handle);

        vk_image_views[attachment_count] = vk_image_view;
        ++attachment_count;
    }

    VkFramebufferCreateInfo create_info = vulkan_helpers::gen_framebuffer_create_info();
    create_info.renderPass      = vk_render_pass;
    create_info.attachmentCount = attachment_count;
    create_info.pAttachments    = vk_image_views;
    create_info.width           = info.width;
    create_info.height          = info.height;
    create_info.layers          = info.depth;

    VkFramebuffer vk_framebuffer = _logical_device->create_framebuffer(create_info);
    if(vk_framebuffer == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    FramebufferHandle fb_handle = _vk_framebuffers.allocate(vk_framebuffer);
    return fb_handle;
}

RenderPassHandle VulkanDeviceContext::create_render_pass(const RenderPassInfo& info)
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
        const SubpassInfo& rend_subpass  = info.subpasses[subpass_idx];
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
        const SubpassDependency& rend_subpass_dep = info.subpass_dependencies[subpass_dep_idx];

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
    if(vk_render_pass == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    RenderPassHandle handle = _vk_render_passes.allocate(vk_render_pass);

    return handle;
}

PipelineLayoutHandle VulkanDeviceContext::create_pipeline_layout(const PipelineLayoutInfo& info)
{
    std::vector<VkDescriptorSetLayout> vk_descriptor_set_layouts;
    vk_descriptor_set_layouts.reserve(info.descriptor_set_layout_count);

    for(size_t idx{0}; idx < info.descriptor_set_layout_count; ++idx)
    {
        vk_descriptor_set_layouts.push_back(get_descriptor_set_layout(info.descriptor_set_layouts[idx]));
    }

    std::vector<VkPushConstantRange> vk_push_constant_ranges;
    vk_push_constant_ranges.reserve(info.push_constant_range_count);

    for(size_t idx{0}; idx < info.push_constant_range_count; ++idx)
    {
        vk_push_constant_ranges.push_back(vulkan_helpers::convert_push_constant_range(info.push_constant_ranges[idx]));
    }

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = vulkan_helpers::gen_pipeline_layout_create_info();
    pipeline_layout_create_info.setLayoutCount         = info.descriptor_set_layout_count;
    pipeline_layout_create_info.pSetLayouts            = vk_descriptor_set_layouts.data();
    pipeline_layout_create_info.pushConstantRangeCount = info.push_constant_range_count;
    pipeline_layout_create_info.pPushConstantRanges    = vk_push_constant_ranges.data();

    VkPipelineLayout pipeline_layout = _logical_device->create_pipeline_layout(pipeline_layout_create_info);
    if(pipeline_layout == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    PipelineLayoutHandle handle = _vk_pipeline_layouts.allocate(pipeline_layout);
    return handle;
}

PipelineHandle VulkanDeviceContext::create_pipeline(const PipelineInfo& info)
{
    const int c_vertex_stage   = 0;
    const int c_fragment_stage = 1;

    VkGraphicsPipelineCreateInfo pipeline_create_info = vulkan_helpers::gen_graphics_pipeline_create_info();
    pipeline_create_info.layout             = get_pipeline_layout(info.layout_handle);
    pipeline_create_info.renderPass         = get_render_pass(info.render_pass_handle);
    pipeline_create_info.subpass            = info.subpass;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex  = 0;

    // Shader stages
    VkPipelineShaderStageCreateInfo shader_create_infos[SHADER_STAGE_COUNT];
    int create_info_idx{ 0 };

    ShaderHandle vertex_shader_handle = info.shaders[c_vertex_stage];
    if(vertex_shader_handle != NULL_HANDLE)
    {
        shader_create_infos[create_info_idx]        = vulkan_helpers::gen_shader_stage_create_info();
        shader_create_infos[create_info_idx].module = *_vk_shaders.get(vertex_shader_handle);
        shader_create_infos[create_info_idx].pName  = "main";
        shader_create_infos[create_info_idx].pSpecializationInfo = nullptr;
        shader_create_infos[create_info_idx].stage  = VK_SHADER_STAGE_VERTEX_BIT;
        ++create_info_idx;
    }

    ShaderHandle fragment_shader_handle = info.shaders[c_fragment_stage];
    if(fragment_shader_handle != NULL_HANDLE)
    {
        shader_create_infos[create_info_idx] = vulkan_helpers::gen_shader_stage_create_info();
        shader_create_infos[create_info_idx].module = *_vk_shaders.get(fragment_shader_handle);
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
        vk_attribute_descs[vertex_attribute_idx].location = info.vertex_attribute_infos[vertex_attribute_idx].shader_location;
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
    if(pipeline == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    PipelineHandle pipeline_handle = _vk_pipelines.allocate(pipeline);
    return pipeline_handle;
}

CommandPoolHandle VulkanDeviceContext::create_command_pool(void)
{
    VkCommandPoolCreateInfo create_info = vulkan_helpers::gen_command_pool_create_info();
    create_info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex        = _logical_device->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkCommandPool vk_command_pool = _logical_device->create_command_pool(create_info);

    CommandPoolHandle pool_handle = _vk_command_pools.allocate(vk_command_pool);

    return pool_handle;
}

DescriptorPoolHandle VulkanDeviceContext::create_descriptor_pool(const DescriptorPoolInfo& info)
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
   if(vk_pool == VK_NULL_HANDLE)
   {
       return NULL_HANDLE;
   }

   DescriptorPoolHandle handle = _vk_descriptor_pools.allocate(vk_pool);

   return handle;
}

DescriptorSetLayoutHandle VulkanDeviceContext::create_descriptor_set_layout(const DescriptorSetLayoutInfo& info)
{
    std::vector<VkDescriptorSetLayoutBinding> vk_descriptor_set_layout_bindings;

    for(size_t idx{0}; idx < info.layout_bindings_count; ++idx)
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
    create_info.bindingCount = info.layout_bindings_count;

    VkDescriptorSetLayout vk_descriptor_set_layout = _logical_device->create_descriptor_set_layout(create_info);
    if(vk_descriptor_set_layout == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    DescriptorSetLayoutHandle layout_handle = _vk_descriptor_set_layouts.allocate(vk_descriptor_set_layout);

    return layout_handle;
}

DescriptorSetHandle VulkanDeviceContext::create_descriptor_set(const DescriptorSetInfo& info)
{
    VkDescriptorPool vk_pool        = get_descriptor_pool(info.pool_handle);
    VkDescriptorSetLayout vk_layout = get_descriptor_set_layout(info.layout_handle);

    std::vector<VkDescriptorSet> vk_descriptor_sets = _logical_device->allocate_descriptor_sets( &vk_layout, 1, vk_pool);

    if(vk_descriptor_sets.empty())
    {
        return NULL_HANDLE;
    }

    VulkanDescriptorSetInfo descriptor_set_info{};
    descriptor_set_info.set           = vk_descriptor_sets[0];
    descriptor_set_info.pool_handle   = info.pool_handle;
    descriptor_set_info.layout_handle = info.layout_handle;

    DescriptorSetHandle handle = _vk_descriptor_set_infos.allocate(descriptor_set_info);
    
    return handle;
}

CommandBufferHandle VulkanDeviceContext::create_command_buffer(CommandPoolHandle pool_handle)
{
    VkCommandPool vk_command_pool = *_vk_command_pools.get(pool_handle);

    auto vk_command_buffers = _logical_device->allocate_command_buffers(1, VK_COMMAND_BUFFER_LEVEL_PRIMARY, vk_command_pool);

    CommandBufferHandle command_buffer_handle = _vk_command_buffers.allocate(vk_command_buffers[0]);

    return command_buffer_handle;
}

void VulkanDeviceContext::destroy_command_buffer(CommandBufferHandle buffer_handle, CommandPoolHandle pool_handle)
{
    VkCommandBuffer vk_command_buffer = *_vk_command_buffers.get(buffer_handle);
    VkCommandPool vk_command_pool     = *_vk_command_pools.get(pool_handle);

    std::vector<VkCommandBuffer> vk_command_buffers{ vk_command_buffer };
    _logical_device->free_command_buffers(vk_command_buffers, vk_command_pool);
}

void VulkanDeviceContext::destroy_command_pool(CommandPoolHandle handle)
{
    VkCommandPool vk_command_pool = *_vk_command_pools.get(handle);

    _logical_device->destroy_command_pool(vk_command_pool);

    _vk_command_pools.deallocate(handle);
}

void VulkanDeviceContext::destroy_descriptor_pool(DescriptorPoolHandle handle)
{
    VkDescriptorPool vk_pool = get_descriptor_pool(handle);
    _logical_device->destroy_descriptor_pool(vk_pool);

    _vk_descriptor_pools.deallocate(handle);
}

void VulkanDeviceContext::destroy_descriptor_set_layout(DescriptorSetLayoutHandle handle)
{
    VkDescriptorSetLayout vk_layout = *_vk_descriptor_set_layouts.get(handle);
    _logical_device->destroy_descriptor_set_layout(vk_layout);

    _vk_descriptor_set_layouts.deallocate(handle);
}

//TODO: Update to handle multiple
void VulkanDeviceContext::destroy_descriptor_set(DescriptorSetHandle descriptor_set_handle)
{
    VulkanDescriptorSetInfo& descriptor_set_info = *_vk_descriptor_set_infos.get(descriptor_set_handle);

    VkDescriptorSet  vk_descriptor_set = descriptor_set_info.set;
    VkDescriptorPool vk_pool = get_descriptor_pool(descriptor_set_info.pool_handle);

    _logical_device->free_descriptor_sets(&vk_descriptor_set, 1, vk_pool);

}

Texture2DHandle VulkanDeviceContext::register_swapchain_image(VkImage swapchain_image, VkFormat format)
{
    VkImageView view = _create_image_view(swapchain_image, format, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
    TextureViewHandle view_handle = _vk_image_views.allocate(view);

    VulkanImageInfo image_info{};
    image_info.image = swapchain_image;
    image_info.view_handle = view_handle;

    TextureHandle handle = _vk_image_infos.allocate(image_info);

    return handle;
}

void VulkanDeviceContext::destroy_buffer(BufferHandle buffer_handle)
{
    VulkanBufferInfo& buffer_info = *_vk_buffer_infos.get(buffer_handle);
    VkDeviceMemory memory = *_vk_memorys.get(buffer_info.memory_handle);

    _logical_device->destroy_buffer(buffer_info.buffer);
    _logical_device->free_memory(memory);

    _vk_memorys.deallocate(buffer_info.memory_handle);
    _vk_buffer_infos.deallocate(buffer_handle);
}

void VulkanDeviceContext::destroy_texture(Texture2DHandle texture_handle)
{
    VulkanImageInfo& image_info = *_vk_image_infos.get(texture_handle);

    if(image_info.sampler_handle != NULL_HANDLE)
    {
        _destroy_sampler(image_info.sampler_handle);
    }

    destroy_image_view(image_info.view_handle);

    VkImage image = image_info.image;
    VkDeviceMemory memory = *_vk_memorys.get(image_info.memory_handle);

    _logical_device->destroy_image(image);
    _logical_device->free_memory(memory);

    _vk_memorys.deallocate(image_info.memory_handle);
    _vk_image_infos.deallocate(texture_handle);
}

void VulkanDeviceContext::destroy_image_view(Texture2DHandle view_handle)
{
    VkImageView image_view = *_vk_image_views.get(view_handle);

    _logical_device->destroy_image_view(image_view);
    _vk_image_views.deallocate(view_handle);
}

void VulkanDeviceContext::destroy_shader(ShaderHandle handle)
{
    VkShaderModule* module = _vk_shaders.get(handle);
    _logical_device->destroy_shader_module(*module);
    _vk_shaders.deallocate(handle);
}

void VulkanDeviceContext::destroy_framebuffer(FramebufferHandle handle)
{
    VkFramebuffer* fb = _vk_framebuffers.get(handle);
    _logical_device->destroy_framebuffer(*fb);
    _vk_framebuffers.deallocate(handle);
}

void VulkanDeviceContext::destroy_render_pass(RenderPassHandle handle)
{
    VkRenderPass* rp = _vk_render_passes.get(handle);
    _logical_device->destroy_render_pass(*rp);
    _vk_render_passes.deallocate(handle);
}

void VulkanDeviceContext::destroy_pipeline_layout(PipelineLayoutHandle handle)
{
    VkPipelineLayout pipeline_layout = *_vk_pipeline_layouts.get(handle);
    _logical_device->destroy_pipeline_layout(pipeline_layout);
    _vk_pipeline_layouts.deallocate(handle);
}

void VulkanDeviceContext::destroy_pipeline(PipelineHandle handle)
{
    VkPipeline pipeline = *_vk_pipelines.get(handle);
    _logical_device->destroy_pipeline(pipeline);
    _vk_pipelines.deallocate(handle);
}

void VulkanDeviceContext::unregister_swapchain_image(Texture2DHandle swapchain_handle)
{
    VulkanImageInfo& info = *_vk_image_infos.get(swapchain_handle);

    destroy_image_view(info.view_handle);

    _vk_image_infos.deallocate(swapchain_handle);
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

void VulkanDeviceContext::bind_descriptor_sets(CommandBufferHandle command_buffer_handle, PipelineBindPoint bind_point, PipelineHandle pipeline_handle, DescriptorSet* descriptor_set, uint32_t descriptor_set_count)
{
    //TODO: Figure out a good array size
    const int c_descriptor_set_max = 16;

    assert(descriptor_set_count <= c_descriptor_set_max);
    assert(descriptor_set_count > 0);

    VkCommandBuffer vk_command_buffer   = get_command_buffer(command_buffer_handle);
    VkPipelineBindPoint vk_bind_point   = vulkan_helpers::convert_pipeline_bind_point(bind_point);
    VkPipelineLayout vk_pipeline_layout = get_pipeline_layout(pipeline_handle);

    VkDescriptorSet vk_descriptor_sets[c_descriptor_set_max];
    for(size_t i = 0; i < descriptor_set_count; ++i)
    {
        vk_descriptor_sets[i] = get_descriptor_set(descriptor_set->handle());
    }

    vkCmdBindDescriptorSets(vk_command_buffer, vk_bind_point, vk_pipeline_layout, 0, descriptor_set_count, vk_descriptor_sets, 0, nullptr);
}

void VulkanDeviceContext::bind_pipeline(CommandBufferHandle buffer_handle, PipelineBindPoint bind_point, PipelineHandle pipeline_handle)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(buffer_handle);
    VkPipeline      vk_pipeline = *_vk_pipelines.get(pipeline_handle);

    vkCmdBindPipeline(vk_command_buffer, vulkan_helpers::convert_pipeline_bind_point(bind_point), vk_pipeline);
}

void VulkanDeviceContext::bind_vertex_buffer(CommandBufferHandle command_buffer_handle, BufferHandle handle)
{
    //TODO: Update to bind more than 1 buffer
    //TODO: Handle non-0 offsets
    //TODO: Handle non-0 first binding
    VkDeviceSize offset = 0;
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);
    VulkanBufferInfo& vertex_buffer_info = *_vk_buffer_infos.get(handle);

    vkCmdBindVertexBuffers(vk_command_buffer, 0, 1, &vertex_buffer_info.buffer, &offset);
}

void VulkanDeviceContext::bind_index_buffer(CommandBufferHandle command_buffer_handle, BufferHandle handle)
{
    //TODO: Handle non-0 offsets
    VkDeviceSize offset = 0;
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);
    VulkanBufferInfo& index_buffer_info = *_vk_buffer_infos.get(handle);

    vkCmdBindIndexBuffer(vk_command_buffer, index_buffer_info.buffer, offset, VK_INDEX_TYPE_UINT32);
}

void VulkanDeviceContext::command_buffer_begin(CommandBufferHandle command_buffer_handle)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);

    VkCommandBufferBeginInfo info =
    {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .pInheritanceInfo = nullptr
    };

    vkBeginCommandBuffer(vk_command_buffer, &info);
}

void VulkanDeviceContext::command_buffer_end(CommandBufferHandle command_buffer_handle)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);

    vkEndCommandBuffer(vk_command_buffer);
}

void VulkanDeviceContext::command_buffer_reset(CommandBufferHandle command_buffer_handle)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);
    vkResetCommandBuffer(vk_command_buffer, 0);
}

void VulkanDeviceContext::copy_buffer_to_buffer(CommandBufferHandle command_buffer_handle, BufferHandle src_handle, BufferHandle dst_handle, const BufferBufferCopyInfo& info)
{
    VkBufferCopy copy =
    {
        .srcOffset = info.src_offset,
        .dstOffset = info.dst_offset,
        .size      = info.size_bytes
    };

    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);
    VulkanBufferInfo& src_buffer_info = *_vk_buffer_infos.get(src_handle);
    VulkanBufferInfo& dst_buffer_info = *_vk_buffer_infos.get(dst_handle);

    vkCmdCopyBuffer(vk_command_buffer, src_buffer_info.buffer, dst_buffer_info.buffer, 1, &copy);
}

void VulkanDeviceContext::copy_buffer_to_image(CommandBufferHandle command_buffer_handle, BufferHandle src_buffer_handle, TextureHandle dst_texture_handle, const BufferImageCopyInfo& info)
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

    VkCommandBuffer  vk_command_buffer = get_command_buffer(command_buffer_handle);
    VulkanBufferInfo& buffer_info = *_vk_buffer_infos.get(src_buffer_handle);
    VulkanImageInfo&  image_info  = *_vk_image_infos.get(dst_texture_handle);

    vkCmdCopyBufferToImage(
        vk_command_buffer,
        buffer_info.buffer,
        image_info.image,
        vulkan_helpers::convert_image_layout(info.image_layout),
        1,
        &copy
    );
}

void VulkanDeviceContext::draw(CommandBufferHandle command_buffer_handle, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);

    vkCmdDraw(vk_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

void VulkanDeviceContext::draw_indexed(CommandBufferHandle command_buffer_handle, uint32_t index_count, uint32_t instance_count, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);

    vkCmdDrawIndexed(vk_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void VulkanDeviceContext::pipeline_barrier(const CommandBufferHandle command_buffer_handle, const PipelineBarrierInfo& info)
{
    VkImageMemoryBarrier vk_image_memory_barriers[8];
    for(size_t barrier_idx{ 0 }; barrier_idx < info.image_memory_barrier_count; ++barrier_idx)
    {
        VulkanImageInfo& image_info = *_vk_image_infos.get(info.image_memory_barriers[barrier_idx].image_handle);

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

    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);
    vkCmdPipelineBarrier(
       vk_command_buffer,
       vulkan_helpers::convert_pipeline_stages(info.src_stages),
       vulkan_helpers::convert_pipeline_stages(info.dst_stages),
       VK_DEPENDENCY_BY_REGION_BIT,
       0, nullptr,
       0, nullptr,
       info.image_memory_barrier_count, vk_image_memory_barriers
    );
}

void VulkanDeviceContext::push_constant(const CommandBufferHandle command_buffer_handle, const PipelineLayoutHandle layout_handle, ShaderStages stages, uint32_t offset, uint32_t size, const void* data)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);
    VkPipelineLayout vk_layout = get_pipeline_layout(layout_handle);
    VkShaderStageFlags shader_stage_flags = vulkan_helpers::convert_shader_stages(stages);

    vkCmdPushConstants(vk_command_buffer, vk_layout, shader_stage_flags, offset, size, data);
}

// TODO: Figure out max viewports
// TODO: Support first viewport
void VulkanDeviceContext::set_viewport(const CommandBufferHandle command_buffer_handle, const ViewportInfo* infos, uint32_t infos_count)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);

    VkViewport vk_viewports[4];
    for(size_t i{0}; i < infos_count; ++i)
    {
        vk_viewports[i].x        = infos[i].x;
        vk_viewports[i].y        = infos[i].y;
        vk_viewports[i].width    = infos[i].width;
        vk_viewports[i].height   = infos[i].height;
        vk_viewports[i].minDepth = infos[i].min_depth;
        vk_viewports[i].maxDepth = infos[i].max_depth;
    }

    vkCmdSetViewport(vk_command_buffer, 0, infos_count, &vk_viewports[0]);
}

void VulkanDeviceContext::set_scissor(const CommandBufferHandle command_buffer_handle, const ViewportInfo* infos, uint32_t infos_count)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);

    VkRect2D vk_scissors[4];
    for(size_t i{0}; i < infos_count; ++i)
    {
        vk_scissors[i].offset.x      = infos[i].x;
        vk_scissors[i].offset.y      = infos[i].y;
        vk_scissors[i].extent.width  = infos[i].width;
        vk_scissors[i].extent.height = infos[i].height;
    }

    vkCmdSetScissor(vk_command_buffer, 0, infos_count, &vk_scissors[0]);
}

void VulkanDeviceContext::begin_render_pass(const CommandBufferHandle command_buffer_handle, const RenderPassHandle render_pass_handle, const FramebufferHandle framebuffer_handle, const RenderArea render_area, const ColourClear clear_colour, const DepthStencilClear clear_depth_stencil )
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);

    VkClearValue vk_clear_values[2];
    vk_clear_values[0].color        = { clear_colour.r, clear_colour.g, clear_colour.b, clear_colour.a };
    vk_clear_values[1].depthStencil = { clear_depth_stencil.depth, clear_depth_stencil.stencil };

    VkRect2D vk_render_area{};
    vk_render_area.extent.width  = render_area.w;
    vk_render_area.extent.height = render_area.h;

    // TODO Get clear values from Framebuffer per attachment
    VkRenderPassBeginInfo vk_render_pass_begin_info{};
    vk_render_pass_begin_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    vk_render_pass_begin_info.pNext           = nullptr,
    vk_render_pass_begin_info.renderPass      = get_render_pass(render_pass_handle),
    vk_render_pass_begin_info.framebuffer     = get_framebuffer(framebuffer_handle),
    vk_render_pass_begin_info.renderArea      = vk_render_area,
    vk_render_pass_begin_info.clearValueCount = 2,
    vk_render_pass_begin_info.pClearValues    = vk_clear_values;

    vkCmdBeginRenderPass(vk_command_buffer, &vk_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanDeviceContext::end_render_pass(const CommandBufferHandle command_buffer_handle)
{
    VkCommandBuffer vk_command_buffer = get_command_buffer(command_buffer_handle);
    vkCmdEndRenderPass(vk_command_buffer);
}

void VulkanDeviceContext::add_descriptor_binding(const DescriptorSetHandle handle, const DescriptorSetBinding& binding)
{
    VkDescriptorSet vk_set = get_descriptor_set(handle);

    VkWriteDescriptorSet write_desc = {};
    write_desc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_desc.pNext            = nullptr;
    write_desc.dstSet           = vk_set;
    write_desc.dstBinding       = binding.slot;
    write_desc.dstArrayElement  = 0;
    write_desc.descriptorCount  = 1;
    write_desc.descriptorType   = vulkan_helpers::convert_descriptor_type(binding.type);

    // TODO update this to be able to write multiple

    switch(binding.type)
    {
        case DescriptorType::COMBINED_IMAGE_SAMPLER:
        case DescriptorType::SAMPLED_IMAGE:
        {
            GPUTexture* texture = static_cast<GPUTexture*>(binding.resource);

            VulkanImageInfo& image_info = *_vk_image_infos.get(texture->handle());

            VkDescriptorImageInfo vk_descriptor_image_info{};
            vk_descriptor_image_info.sampler = *_vk_samplers.get(image_info.sampler_handle);
            vk_descriptor_image_info.imageView = *_vk_image_views.get(image_info.view_handle);
            vk_descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            write_desc.pImageInfo = &vk_descriptor_image_info;

            _logical_device->update_descriptor_sets( &write_desc, 1 );

            break;
        }

        case DescriptorType::UNIFORM_BUFFER:
        {
            GPUBuffer* buffer = static_cast<GPUBuffer*>(binding.resource);

            VulkanBufferInfo& buffer_info = *_vk_buffer_infos.get(buffer->handle());

            VkDescriptorBufferInfo vk_buffer_info{};
            vk_buffer_info.buffer  = buffer_info.buffer;
            vk_buffer_info.offset = 0;
            vk_buffer_info.range   = buffer->bytes();

            write_desc.pBufferInfo = &vk_buffer_info;

            _logical_device->update_descriptor_sets( &write_desc, 1 );

            break;
        }
        default:
        {
            break;
        }
    }
}

void* VulkanDeviceContext::map_buffer_memory(BufferHandle handle, size_t bytes)
{
    void* mapped = nullptr;
    VulkanBufferInfo& buffer_info = *_vk_buffer_infos.get(handle);
    VkDeviceMemory memory = *_vk_memorys.get(buffer_info.memory_handle);
    _logical_device->map_memory(memory, bytes, 0, &mapped);
    return mapped;
}

void VulkanDeviceContext::unmap_buffer_memory(BufferHandle handle)
{
    VulkanBufferInfo& buffer_info = *_vk_buffer_infos.get(handle);
    VkDeviceMemory memory = *_vk_memorys.get(buffer_info.memory_handle);
    _logical_device->unmap_memory(memory);
}

void* VulkanDeviceContext::map_image_memory(TextureHandle handle, size_t bytes)
{
    void* mapped = nullptr;
    VulkanImageInfo& image_info = *_vk_image_infos.get(handle);
    VkDeviceMemory memory = *_vk_memorys.get(image_info.memory_handle);
    _logical_device->map_memory(memory, bytes, 0, &mapped);
    return mapped;
}

void VulkanDeviceContext::unmap_image_memory(TextureHandle handle)
{
    VulkanImageInfo& image_info = *_vk_image_infos.get(handle);
    VkDeviceMemory memory = *_vk_memorys.get(image_info.memory_handle);
    _logical_device->unmap_memory(memory);
}

VkDeviceMemory VulkanDeviceContext::get_memory(MemoryHandle handle) const
{
    return *_vk_memorys.get(handle);
}

VkShaderModule VulkanDeviceContext::get_shader(const ShaderHandle handle) const
{
    return *_vk_shaders.get(handle);
}

VkFramebuffer  VulkanDeviceContext::get_framebuffer(const FramebufferHandle handle) const
{
    return *_vk_framebuffers.get(handle);
}

VkRenderPass   VulkanDeviceContext::get_render_pass(const RenderPassHandle handle) const
{
    return *_vk_render_passes.get(handle);
}

VkCommandBuffer VulkanDeviceContext::get_command_buffer(const CommandBufferHandle handle) const
{
    return *_vk_command_buffers.get(handle);
}

VkPipelineLayout VulkanDeviceContext::get_pipeline_layout(const PipelineLayoutHandle handle) const
{
    return *_vk_pipeline_layouts.get(handle);
}

VkDescriptorSetLayout VulkanDeviceContext::get_descriptor_set_layout(const DescriptorSetLayoutHandle handle) const
{
    return *_vk_descriptor_set_layouts.get(handle);
}

VkDescriptorSet VulkanDeviceContext::get_descriptor_set(const DescriptorSetHandle handle) const
{
    VulkanDescriptorSetInfo* info = _vk_descriptor_set_infos.get(handle);

    return info->set;
}

VkDescriptorPool VulkanDeviceContext::get_descriptor_pool(const DescriptorPoolHandle handle) const
{
    return *_vk_descriptor_pools.get(handle);
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

BufferHandle VulkanDeviceContext::_create_buffer_internal(size_t bytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties)
{
    uint32_t queue_family_index = _logical_device->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkBufferCreateInfo create_info    = vulkan_helpers::gen_buffer_create_info();
    create_info.size                  = bytes;
    create_info.usage                 = usage;
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices   = &queue_family_index;

    VkBuffer buffer = _logical_device->create_buffer(create_info);

    VkMemoryRequirements memory_reqs = _logical_device->get_buffer_memory_reqs(buffer);

    VkMemoryAllocateInfo alloc_info = vulkan_helpers::gen_memory_allocate_info();
    alloc_info.allocationSize = memory_reqs.size;
    alloc_info.memoryTypeIndex = _logical_device->find_memory_type(memory_reqs.memoryTypeBits, memory_properties);

    VkDeviceMemory memory = _logical_device->allocate_memory(alloc_info);

    _logical_device->bind_buffer_memory(buffer, memory);

    VulkanBufferInfo buffer_info{};
    buffer_info.buffer = buffer;
    buffer_info.memory_handle = _vk_memorys.allocate(memory);

    BufferHandle handle = _vk_buffer_infos.allocate(buffer_info);

    return handle;
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

void VulkanDeviceContext::_destroy_sampler(SamplerHandle handle)
{
    VkSampler sampler = *_vk_samplers.get(handle);

    _logical_device->destroy_sampler(sampler);
    _vk_samplers.deallocate(handle);
}
