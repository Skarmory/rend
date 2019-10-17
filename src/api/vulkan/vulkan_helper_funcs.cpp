#include "vulkan_helper_funcs.h"
#include "gpu_texture_base.h"
#include "rend_defs.h"

#include <unordered_set>

using namespace rend;

std::unordered_set<VkFormat> depth_formats =
{
    VK_FORMAT_D16_UNORM,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_X8_D24_UNORM_PACK32,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D32_SFLOAT_S8_UINT
};

std::unordered_set<VkFormat> stencil_formats =
{
    VK_FORMAT_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT_S8_UINT
};

VkImageType vulkan_helpers::find_image_type(VkExtent3D extent)
{
    if(extent.depth > 1)
        return VK_IMAGE_TYPE_3D;

    if(extent.height > 1)
        return VK_IMAGE_TYPE_2D;

    return VK_IMAGE_TYPE_1D;
}

VkImageViewType vulkan_helpers::find_image_view_type(VkImageType image_type, bool array, bool cube)
{
    switch(image_type)
    {
        case VK_IMAGE_TYPE_1D:
        {
            return (array ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D);
        }
        case VK_IMAGE_TYPE_2D:
        {
            return (array ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D);
        }
        case VK_IMAGE_TYPE_3D:
        {
            return (cube && array ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : (cube ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_3D));
        }
        case VK_IMAGE_TYPE_RANGE_SIZE:
        case VK_IMAGE_TYPE_MAX_ENUM:
            return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }

    return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

VkImageAspectFlags vulkan_helpers::find_image_aspects(VkFormat format)
{
    VkImageAspectFlags flags = 0;
    bool is_depth_format   = depth_formats.find(format) != depth_formats.end();
    bool is_stencil_format = stencil_formats.find(format) != stencil_formats.end();

    if(!is_depth_format && !is_stencil_format)
    {
        flags |= VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else
    {
        if(is_depth_format)
            flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        if(is_stencil_format)
            flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    return flags;
}

VkFormat vulkan_helpers::convert_texture_format(TextureFormat format)
{
    switch(format)
    {
        case TextureFormat::R8G8B8A8: return VK_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::R16G16B16A16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case TextureFormat::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
        case TextureFormat::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
    }

    return VK_FORMAT_MAX_ENUM;
}

VkShaderStageFlags vulkan_helpers::convert_shader_stage(ShaderType type)
{
    switch(type)
    {
        case ShaderType::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderType::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
    }
}
