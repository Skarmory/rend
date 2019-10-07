#include "shader.h"

#include "device_context.h"
#include "logical_device.h"

using namespace rend;

VulkanShader::VulkanShader(DeviceContext& context)
    : _context(context),
      _vk_module(VK_NULL_HANDLE)
{
}

VulkanShader::~VulkanShader(void)
{
    vkDestroyShaderModule(_context.get_device()->get_handle(), _vk_module, nullptr);
}

VkShaderModule VulkanShader::get_handle(void) const
{
    return _vk_module;
}

StatusCode VulkanShader::create_shader_api(const void* code, uint32_t size_bytes, ShaderType type)
{
    UU(type);

    if(_vk_module != VK_NULL_HANDLE)
        return StatusCode::ALREADY_CREATED;

    VkShaderModuleCreateInfo info =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = size_bytes,
        .pCode = static_cast<const uint32_t*>(code)
    };

    if(vkCreateShaderModule(_context.get_device()->get_handle(), &info, nullptr, &_vk_module) != VK_SUCCESS)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}
