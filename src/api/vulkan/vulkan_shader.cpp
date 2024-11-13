#include "api/vulkan/vulkan_shader.h"

using namespace rend;

VulkanShader::VulkanShader(const std::string& name, size_t size_bytes, ShaderStage type, VkShaderModule vk_handle)
    :
        Shader(name, size_bytes, type),
        _vk_handle(vk_handle)
{
}

VkShaderModule VulkanShader::vk_handle(void) const
{
    return _vk_handle;
}
