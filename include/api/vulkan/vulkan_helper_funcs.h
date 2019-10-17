#ifdef USE_VULKAN
#ifndef REND_VULKAN_HELPER_FUNCS_H
#define REND_VULKAN_HELPER_FUNCS_H

#include <vulkan.h>

namespace rend
{

enum class ShaderType;
enum class TextureFormat;

namespace vulkan_helpers
{

VkImageType        find_image_type(VkExtent3D extent);
VkImageViewType    find_image_view_type(VkImageType image_type, bool array, bool cube);
VkImageAspectFlags find_image_aspects(VkFormat format);
VkFormat           convert_texture_format(TextureFormat format);
VkShaderStageFlags convert_shader_stage(ShaderType type);

}

}

#endif
#endif
