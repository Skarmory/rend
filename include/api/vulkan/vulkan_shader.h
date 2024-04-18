#ifndef REND_API_VULKAN_VULKAN_SHADER_H
#define REND_API_VULKAN_VULKAN_SHADER_H

#include "core/shader.h"

#include <string>
#include <vulkan.h>

namespace rend
{

class VulkanShader : public Shader
{
public:
    VulkanShader(const std::string& name, size_t size_bytes, ShaderStage type, RendHandle rend_handle, VkShaderModule vk_handle);
    ~VulkanShader(void) = default;

    VkShaderModule vk_handle(void) const;

private:
    VkShaderModule _vk_handle{ VK_NULL_HANDLE };
};

}

#endif
