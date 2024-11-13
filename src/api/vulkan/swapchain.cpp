#include "api/vulkan/swapchain.h"

#include "api/vulkan/fence.h"
#include "api/vulkan/logical_device.h"
#include "api/vulkan/physical_device.h"
#include "api/vulkan/vulkan_device_context.h"
#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_instance.h"
#include "api/vulkan/vulkan_semaphore.h"
#include "api/vulkan/vulkan_texture.h"
#include "core/window.h"
#include "core/logging/log_defs.h"
#include "core/logging/log_manager.h"
#include <cassert>
#include <limits>
#include <sstream>

using namespace rend;

namespace
{
    std::string swapchain_params_to_string(const VkSwapchainCreateInfoKHR& create_info)
    {
        std::string s = "{ ";
        s += "min image count: " + create_info.minImageCount;
        s += " }";
        return s;
    }
}

Swapchain::Swapchain(uint32_t desired_images, VulkanDeviceContext& ctx)
    :
        _desired_image_count(desired_images),
        _ctx(&ctx)
{
    _create();
}

Swapchain::~Swapchain(void)
{
    _clean_up_images();
    _ctx->get_device()->destroy_swapchain(_vk_swapchain);
}

StatusCode Swapchain::recreate(void)
{
#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SWAPCHAIN | Recreate");
#endif

    ++_create_count;
    _clean_up_images();
    return _create();
}

Format Swapchain::get_format(void) const
{
    return vulkan_helpers::convert_format(_surface_format.format);
}

VkExtent2D Swapchain::get_extent(void) const
{
    return _vk_extent;
}

VkSwapchainKHR Swapchain::vk_handle(void) const
{
    return _vk_swapchain;
}

const VulkanTexture& Swapchain::get_backbuffer(uint32_t backbuffer_idx) const
{
    auto handle = _backbuffer_resources.backbuffer_handles[backbuffer_idx]; 
    return *_backbuffer_resources.backbuffer_textures.get(handle);
}

TextureInfo Swapchain::get_backbuffer_texture_info(void) const
{
    return _backbuffer_texture_info;
}

StatusCode Swapchain::acquire(SwapchainAcquire** out_acquire)
{
    *out_acquire = &_backbuffer_resources.swapchain_acquires.next();

    VkResult result = _ctx->get_device()->acquire_next_image(
        this, std::numeric_limits<uint64_t>::max(), (*out_acquire)->acquire_semaphore, nullptr, &_image_idx
    );

#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SWAPCHAIN | Acquired image index: " + std::to_string(_image_idx));
#endif

    if(result != VK_SUCCESS)
    {
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return StatusCode::SWAPCHAIN_OUT_OF_DATE;
        }
        else
        {
            return StatusCode::SWAPCHAIN_ACQUIRE_ERROR;
        }
    }

    (*out_acquire)->image_idx = _image_idx;

    return StatusCode::SUCCESS;
}

StatusCode Swapchain::present(const SwapchainAcquire& acquisition, QueueType type /*, const std::vector<Semaphore*>& wait_sems*/)
{
    std::vector<VkResult> results(1);

#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SWAPCHAIN | Presenting image index: " + std::to_string(acquisition.image_idx));
#endif

    if(_ctx->get_device()->queue_present(type, { acquisition.present_semaphore }, { this }, { acquisition.image_idx }, results))
    {
        return StatusCode::FAILURE;
    }

    if(results[0] != VK_SUCCESS)
    {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode Swapchain::_create(void)
{
    const PhysicalDevice& physical_device = _ctx->get_device()->get_physical_device();

    VkSurfaceCapabilitiesKHR surface_caps = physical_device.get_surface_capabilities(_ctx->vulkan_instance());

    if(physical_device.get_surface_formats().empty())
    {
        return StatusCode::SWAPCHAIN_NO_SURFACE_FORMATS_FOUND;
    }

    if(physical_device.get_surface_present_modes().empty())
    {
        return StatusCode::SWAPCHAIN_NO_SURFACE_PRESENT_MODES_FOUND;
    }

    _surface_format = _find_surface_format(physical_device.get_surface_formats());
    auto present_mode = _find_present_mode(physical_device.get_surface_present_modes());
    auto found_image_count = _find_image_count(_desired_image_count, surface_caps);

    _vk_extent = surface_caps.currentExtent;

    VkSwapchainKHR old_swapchain = _vk_swapchain;

    VkSwapchainCreateInfoKHR create_info = vulkan_helpers::gen_swapchain_create_info();
    create_info.surface               = _ctx->vulkan_instance().surface();
    create_info.minImageCount         = found_image_count;
    create_info.imageFormat           = _surface_format.format;
    create_info.imageColorSpace       = _surface_format.colorSpace;
    create_info.imageExtent           = surface_caps.currentExtent;
    create_info.imageArrayLayers      = 1;
    create_info.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    create_info.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode           = present_mode;
    create_info.oldSwapchain          = old_swapchain;

#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SWAPCHAIN | Creating swapchain with params: " + ::swapchain_params_to_string(create_info));
#endif

    _vk_swapchain = _ctx->get_device()->create_swapchain(create_info);
    if(_vk_swapchain == VK_NULL_HANDLE)
    {
        return StatusCode::FAILURE;
    }

    if(old_swapchain != VK_NULL_HANDLE)
    {
        _ctx->get_device()->destroy_swapchain(old_swapchain);
    }

    _ctx->get_device()->get_swapchain_images(_vk_swapchain, _backbuffer_resources.vk_swapchain_images);

    _backbuffer_texture_info =
    {
        .width = _vk_extent.width,
        .height = _vk_extent.height,
        .depth = 0,
        .use_size_ratio = true,
        .size_ratio = SizeRatio::FULL,
        .mips = 0,
        .layers = 1,
        .format = get_format(),
        .layout = ImageLayout::UNDEFINED,
        .samples = MSAASamples::MSAA_1X,
        .usage = ImageUsage::COLOUR_ATTACHMENT | ImageUsage::TRANSFER_DST
    };

    // Set up backbuffer textures as rend objects
    for(uint32_t i = 0; i < _backbuffer_resources.vk_swapchain_images.size(); ++i)
    {
        SwapchainAcquire acquisition;

        const std::string number_name = std::to_string(i);
        const std::string resize_count_name = std::to_string(_create_count);
        const std::string backbuffer_name = "backbuffer ";
        std::string name;

        name = backbuffer_name + number_name + " resize#" + resize_count_name;
        _backbuffer_resources.backbuffer_handles.push_back(_register_swapchain_image(name, _backbuffer_resources.vk_swapchain_images[i]));

        name = backbuffer_name + number_name + " acquire semaphore";
        acquisition.acquire_semaphore = new Semaphore(name, *_ctx, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        name = backbuffer_name + number_name + " present semaphore";
        acquisition.present_semaphore = new Semaphore(name, *_ctx, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

        name = backbuffer_name + number_name + " acquire fence";
        acquisition.acquire_fence = new Fence(name, true, *_ctx);

        _backbuffer_resources.swapchain_acquires.add(acquisition);
    }

    return StatusCode::SUCCESS;
}

void Swapchain::_clean_up_images(void)
{
#if DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SWAPCHAIN | Cleaning up swapchain resources");
#endif

    for(uint32_t i = 0; i < _backbuffer_resources.backbuffer_handles.size(); ++i)
    {
        _unregister_swapchain_image(_backbuffer_resources.backbuffer_handles[i]);
    }

    for(uint32_t i = 0; i < _backbuffer_resources.swapchain_acquires.count(); ++i)
    {
        SwapchainAcquire& acquire = _backbuffer_resources.swapchain_acquires.next();

        delete acquire.acquire_fence;
        delete acquire.acquire_semaphore;
        delete acquire.present_semaphore;
    }

    _backbuffer_resources.backbuffer_handles.clear();
    _backbuffer_resources.swapchain_acquires.clear();
    _backbuffer_resources.vk_swapchain_images.clear();
}

VkSurfaceFormatKHR Swapchain::_find_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats)
{
    if(surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
    {
        return VkSurfaceFormatKHR
        {
            .format = VK_FORMAT_B8G8R8A8_UNORM,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };
    }

    for(VkSurfaceFormatKHR surface_format : surface_formats)
    {
        if(surface_format.format == VK_FORMAT_B8G8R8A8_UNORM && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return surface_format;
        }
    }

    return surface_formats[0];
}

VkPresentModeKHR Swapchain::_find_present_mode(const std::vector<VkPresentModeKHR>& present_modes)
{
    VkPresentModeKHR chosen = VK_PRESENT_MODE_FIFO_KHR;
    for(VkPresentModeKHR present_mode : present_modes)
    {
        if(present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return present_mode;
        }

        if(present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            chosen = present_mode; 
        }
    }

    return chosen;
}

uint32_t Swapchain::_find_image_count(uint32_t desired_images, const VkSurfaceCapabilitiesKHR& surface_caps)
{
    if(surface_caps.maxImageCount > 0 && desired_images > surface_caps.maxImageCount)
    {
        return surface_caps.maxImageCount;
    }
    else if(surface_caps.minImageCount > 0 && desired_images < surface_caps.minImageCount + 1)
    {
        return surface_caps.minImageCount + 1;
    }

    return desired_images;
}

TextureHandle Swapchain::_register_swapchain_image(const std::string& name, VkImage image)
{
    VulkanImageInfo vk_image_info = _ctx->register_swapchain_image(image, _surface_format.format);
    auto rend_handle = _backbuffer_resources.backbuffer_textures.acquire();
    auto* backbuffer_texture = _backbuffer_resources.backbuffer_textures.get(rend_handle);
    backbuffer_texture = new(backbuffer_texture) VulkanTexture(name, _backbuffer_texture_info, vk_image_info);

#ifdef DEBUG
    _ctx->set_debug_name(name, VK_OBJECT_TYPE_IMAGE, (uint64_t)vk_image_info.image);
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SWAPCHAIN | Registered swapchain image: " + name);
#endif

    return rend_handle;
}

void Swapchain::_unregister_swapchain_image(TextureHandle texture_handle)
{
    auto* texture = _backbuffer_resources.backbuffer_textures.get(texture_handle);
    _ctx->unregister_swapchain_image(texture->vk_image_info());

#ifdef DEBUG
    core::logging::LogManager::write(core::logging::C_RENDERER_LOG_CHANNEL_NAME, "SWAPCHAIN | Unregistered swapchain image: " + texture->name());
#endif

    _backbuffer_resources.backbuffer_textures.release(texture_handle);
}
