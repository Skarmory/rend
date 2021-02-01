#include "vulkan_device_context.h"

#include "gpu_memory_interface.h"
#include "logical_device.h"
#include "physical_device.h"
#include "window.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_instance.h"

#include <cassert>
#include <GLFW/glfw3.h>

using namespace rend;
using namespace rend::vkal;
using namespace rend::vkal::memory;

VulkanDeviceContext::VulkanDeviceContext(void)
{
    assert(_service == nullptr);
    _service = this;

    _vk_buffers.set_unique_key(_data_array_unique_key++);
    _vk_images.set_unique_key(_data_array_unique_key++);
    _vk_image_views.set_unique_key(_data_array_unique_key++);
    _vk_samplers.set_unique_key(_data_array_unique_key++);
    _vk_memorys.set_unique_key(_data_array_unique_key++);
    _vk_shaders.set_unique_key(_data_array_unique_key++);
}

PhysicalDevice* VulkanDeviceContext::gpu(void) const
{
    return _chosen_gpu;
}

//GPUMemoryInterface* VulkanDeviceContext::memory_interface(void) const
//{
//    return _memory_interface;
//}

LogicalDevice* VulkanDeviceContext::get_device(void) const
{
    return _logical_device;
}

StatusCode VulkanDeviceContext::create(void)
{
    assert(_chosen_gpu == nullptr && _logical_device == nullptr && "Attempt to create a VulkanDeviceContext that has already been created.");

    // Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    VulkanInstance::instance().enumerate_physical_devices(physical_devices);

    for(size_t physical_device_index = 0; physical_device_index < physical_devices.size(); physical_device_index++)
    {
        PhysicalDevice* pdev = new PhysicalDevice;
        pdev->create(physical_device_index, physical_devices[physical_device_index]);
        _physical_devices.push_back(pdev);
    }

    return StatusCode::SUCCESS;
}

void VulkanDeviceContext::destroy(void)
{
    for (auto& handle : _vk_memorys)
    {
        _logical_device->free_memory(*_vk_memorys.get(handle));
    }

    for (auto& handle : _vk_buffers)
    {
        _logical_device->destroy_buffer(*_vk_buffers.get(handle));
    }

    for (auto& handle : _vk_samplers)
    {
        _logical_device->destroy_sampler(*_vk_samplers.get(handle));
    }

    for (auto& handle : _vk_image_views)
    {
        _logical_device->destroy_image_view(*_vk_image_views.get(handle));
    }

    for (auto& handle : _vk_images)
    {
        _logical_device->destroy_image(*_vk_images.get(handle));
    }

    for(auto& handle : _vk_shaders)
    {
        _logical_device->destroy_shader_module(*_vk_shaders.get(handle));
    }

    //for(size_t physical_device_index = 0; physical_device_index < _physical_devices.size(); physical_device_index++)
    for(auto physical_device : _physical_devices)
    {
        physical_device->destroy();
        delete physical_device;
    }

    _logical_device = nullptr;
    _chosen_gpu     = nullptr;
    _physical_devices.clear();
    _data_array_unique_key = 1;
    _handle_to_memory_handle.clear();
    _texture_handle_to_view_handle.clear();
    _texture_handle_to_sampler_handle.clear();

    //TODO: Clear data arrays (pending testing of DataArray::clear() function
}

StatusCode VulkanDeviceContext::choose_gpu(const VkPhysicalDeviceFeatures& desired_features)
{
    if(_chosen_gpu)
    {
        return StatusCode::ALREADY_CREATED;
    }

    _chosen_gpu = _find_physical_device(desired_features);

    if(!_chosen_gpu)
    {
        return StatusCode::CONTEXT_GPU_WITH_DESIRED_FEATURES_NOT_FOUND;
    }

    return StatusCode::SUCCESS;
}

StatusCode VulkanDeviceContext::create_device(const VkQueueFlags desired_queues)
{
    if(_logical_device)
    {
        return StatusCode::ALREADY_CREATED;
    }

    if(!_chosen_gpu)
    {
        return StatusCode::CONTEXT_GPU_NOT_CHOSEN;
    }

    if(!_chosen_gpu->create_logical_device(desired_queues))
    {
        return StatusCode::CONTEXT_DEVICE_CREATE_FAILURE;
    }

    //_memory_interface = new GPUMemoryInterface;
    //_memory_interface->create(*_chosen_gpu);
    _logical_device = _chosen_gpu->get_logical_device();

    return StatusCode::SUCCESS;
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

    auto handle = _vk_images.allocate(image);
    auto view_handle = _vk_image_views.allocate(view);
    auto mem_handle = _vk_memorys.allocate(memory);
    auto sampler_handle = _vk_samplers.allocate(sampler);

    _handle_to_memory_handle[handle] = mem_handle;
    _texture_handle_to_view_handle[handle] = view_handle;
    _texture_handle_to_sampler_handle[handle] = sampler_handle;

    return handle;
}

ShaderHandle VulkanDeviceContext::create_shader(const ShaderType type, const void* code, const size_t bytes)
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
        TextureViewHandle view_handle = _texture_handle_to_view_handle.at(attachment_handle);
        VkImageView vk_image_view = *_vk_image_views.get(view_handle);

        vk_image_views[idx] = vk_image_view;
    }

    if(info.depth_buffer_handle != NULL_HANDLE)
    {
        TextureViewHandle view_handle = _texture_handle_to_view_handle.at(info.depth_buffer_handle);
        VkImageView vk_image_view = *_vk_image_views.get(view_handle);

        vk_image_views[attachment_count] =vk_image_view;
        ++attachment_count;
    }

    VkFramebufferCreateInfo create_info = vulkan_helpers::gen_framebuffer_create_info();
    create_info.renderPass      = vk_render_pass;
    create_info.attachmentCount = attachment_count;
    create_info.pAttachments    = vk_image_views;
    create_info.width           = info.width;
    create_info.height          = info.height;
    create_info.layers          = info.depth;

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    VkFramebuffer vk_framebuffer = ctx.get_device()->create_framebuffer(create_info);
    if(vk_framebuffer == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    FramebufferHandle fb_handle = _vk_framebuffers.allocate(vk_framebuffer);
    return fb_handle;
}

RenderPassHandle VulkanDeviceContext::create_render_pass(const RenderPassInfo& info)
{
    VkSubpassDescription vk_subpass_descs[rend::constants::max_subpasses];
    VkSubpassDependency  vk_subpass_deps[rend::constants::max_subpasses + 1];
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

            //vk_subpass.resolveAttachmentCount = rend_subpass.resolve_attachment_infos_count;
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
        static_cast<uint32_t>(info.subpass_dependency_count - 1),
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
    create_info.attachmentCount = static_cast<uint32_t>(info.attachment_infos_count);
    create_info.pAttachments    = &vk_attachment_descs[0];
    create_info.subpassCount    = static_cast<uint32_t>(info.subpasses_count);
    create_info.pSubpasses      = &vk_subpass_descs[0];
    create_info.dependencyCount = static_cast<uint32_t>(info.subpasses_count + 1);
    create_info.pDependencies   = &vk_subpass_deps[0];

    auto& ctx = static_cast<VulkanDeviceContext&>(DeviceContext::instance());
    VkRenderPass vk_render_pass = ctx.get_device()->create_render_pass(create_info);
    if(vk_render_pass == VK_NULL_HANDLE)
    {
        return NULL_HANDLE;
    }

    RenderPassHandle handle = _vk_render_passes.allocate(vk_render_pass);

    return handle;
}

Texture2DHandle VulkanDeviceContext::register_swapchain_image(VkImage swapchain_image, VkFormat format)
{
    Texture2DHandle image_handle = _vk_images.allocate(swapchain_image);

    VkImage* image = _vk_images.get(image_handle);
    VkImageView view = _create_image_view(*image, format, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
    TextureViewHandle view_handle = _vk_image_views.allocate(view);
    _texture_handle_to_view_handle[image_handle] = view_handle;

    return image_handle;
}

void VulkanDeviceContext::destroy_buffer(BufferHandle buffer_handle)
{
    auto it = _handle_to_memory_handle.find(buffer_handle);
    if (it == _handle_to_memory_handle.end())
    {
        return;
    }

    MemoryHandle mem_handle = it->second;
    VkBuffer* buffer = _vk_buffers.get(buffer_handle);
    VkDeviceMemory* memory = _vk_memorys.get(mem_handle);

    if (buffer)
    {
        _logical_device->destroy_buffer(*buffer);
        _vk_buffers.deallocate(buffer_handle);
    }

    if (memory)
    {
        // TODO: Eventually I want to conserve memory usage and pack buffers
        //       into memory blocks. So this will have to be removed.
        _logical_device->free_memory(*memory);
        _vk_memorys.deallocate(mem_handle);
    }

    _handle_to_memory_handle.erase(buffer_handle);
}

void VulkanDeviceContext::destroy_texture(Texture2DHandle texture_handle)
{
    _destroy_sampler(texture_handle);
    destroy_image_view(texture_handle);

    auto it = _handle_to_memory_handle.find(texture_handle);
    if (it == _handle_to_memory_handle.end())
    {
        return;
    }

    MemoryHandle mem_handle = it->second;
    VkImage* image = _vk_images.get(texture_handle);
    VkDeviceMemory* memory = _vk_memorys.get(mem_handle);

    if (image)
    {
        _logical_device->destroy_image(*image);
        _vk_images.deallocate(texture_handle);
    }

    if (memory)
    {
        _logical_device->free_memory(*memory);
        _vk_memorys.deallocate(mem_handle);
    }

    _handle_to_memory_handle.erase(texture_handle);
}

void VulkanDeviceContext::destroy_image_view(Texture2DHandle texture_handle)
{
    auto it = _texture_handle_to_view_handle.find(texture_handle);
    if (it == _texture_handle_to_view_handle.end())
    {
        return;
    }

    TextureViewHandle view_handle = it->second;
    VkImageView* image_view = _vk_image_views.get(view_handle);

    if (image_view)
    {
        _logical_device->destroy_image_view(*image_view);
        _vk_image_views.deallocate(view_handle);
    }

    _texture_handle_to_view_handle.erase(texture_handle);
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

void VulkanDeviceContext::unregister_swapchain_image(Texture2DHandle swapchain_handle)
{
    auto it = _texture_handle_to_view_handle.find(swapchain_handle);
    if (it != _texture_handle_to_view_handle.end())
    {
        TextureViewHandle view_handle = it->second;
        VkImageView* image_view = _vk_image_views.get(view_handle);
        if (image_view)
        {
            _logical_device->destroy_image_view(*image_view);
            _vk_image_views.deallocate(view_handle);
        }
    }

    _texture_handle_to_view_handle.erase(swapchain_handle);
    _vk_images.deallocate(swapchain_handle);
}

VkBuffer VulkanDeviceContext::get_buffer(VertexBufferHandle handle) const
{
    return *_vk_buffers.get(handle);
}

VkImage VulkanDeviceContext::get_image(Texture2DHandle handle) const
{
    return *_vk_images.get(handle);
}

VkImageView VulkanDeviceContext::get_image_view(Texture2DHandle handle) const
{
    TextureViewHandle view_handle = _texture_handle_to_view_handle.at(handle);
    return *_vk_image_views.get(view_handle);
}

VkSampler VulkanDeviceContext::get_sampler(Texture2DHandle handle) const
{
    SamplerHandle sampler_handle = _texture_handle_to_sampler_handle.at(handle);
    return *_vk_samplers.get(sampler_handle);
}

VkDeviceMemory VulkanDeviceContext::get_memory(HandleType handle) const
{
    MemoryHandle mem_handle = _handle_to_memory_handle.at(handle);
    return *_vk_memorys.get(mem_handle);
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

    BufferHandle handle = _vk_buffers.allocate(buffer);
    MemoryHandle mem_handle = _vk_memorys.allocate(memory);

    _handle_to_memory_handle[handle] = mem_handle;

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

void VulkanDeviceContext::_destroy_sampler(TextureHandle handle)
{
    auto it = _texture_handle_to_sampler_handle.find(handle);
    if (it == _texture_handle_to_sampler_handle.end())
    {
        return;
    }

    SamplerHandle sampler_handle = it->second;

    if (VkSampler* sampler = _vk_samplers.get(sampler_handle); sampler_handle)
    {
        _texture_handle_to_sampler_handle.erase(handle);
        _logical_device->destroy_sampler(*sampler);
        _vk_samplers.deallocate(sampler_handle);
    }
}
