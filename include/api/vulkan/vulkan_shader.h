#ifndef REND_VULKAN_SHADER_H
#define REND_VULKAN_SHADER_H

#include "shader_base.h"
#include "rend_defs.h"

#include <vulkan.h>

namespace rend
{

class VulkanShader : public ShaderBase
{
public:
    VulkanShader(void) = default;
    ~VulkanShader(void);

    VulkanShader(const VulkanShader&)            = delete;
    VulkanShader(VulkanShader&&)                 = delete;
    VulkanShader& operator=(const VulkanShader&) = delete;
    VulkanShader& operator=(VulkanShader&&)      = delete;

    VkShaderModule get_handle(void) const;

protected:
    StatusCode create_shader_api(const void* code, uint32_t size_bytes, ShaderType type);

private:
    VkShaderModule _vk_module { VK_NULL_HANDLE };
};

}

#endif
