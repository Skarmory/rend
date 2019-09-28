#include "shader.h"

#include "device_context.h"
#include "logical_device.h"

using namespace rend;

Shader::Shader(DeviceContext* context)
    : _context(context),
      _type(ShaderType::UNKNOWN),
      _vk_module(VK_NULL_HANDLE)
{
}

bool Shader::create_shader(const void* code, uint32_t size_bytes, ShaderType type)
{
    if(_vk_module != VK_NULL_HANDLE)
        return false;

    VkShaderModuleCreateInfo info =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = size_bytes,
        .pCode = static_cast<const uint32_t*>(code)
    };

    if(vkCreateShaderModule(_context->get_device()->get_handle(), &info, nullptr, &_vk_module) != VK_SUCCESS)
        return false;

    _type = type;

    return true;
}

Shader::~Shader(void)
{
    vkDestroyShaderModule(_context->get_device()->get_handle(), _vk_module, nullptr);
}

VkShaderModule Shader::get_handle(void) const
{
    return _vk_module;
}

ShaderType Shader::get_shader_type(void) const
{
    return _type;
}
