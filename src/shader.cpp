#include "shader.h"

#include "logical_device.h"
#include "utils.h"

using namespace rend;

Shader::Shader(LogicalDevice* device, const void* code, uint32_t size_bytes, ShaderType type)
    : _device(device), _type(type)
{
    VkShaderModuleCreateInfo info =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = size_bytes,
        .pCode = static_cast<const uint32_t*>(code)
    };

    VULKAN_DEATH_CHECK(vkCreateShaderModule(_device->get_handle(), &info, nullptr, &_vk_module), "Failed to create shader");
}

Shader::~Shader(void)
{
    vkDestroyShaderModule(_device->get_handle(), _vk_module, nullptr);
}

VkShaderModule Shader::get_handle(void) const
{
    return _vk_module;
}
