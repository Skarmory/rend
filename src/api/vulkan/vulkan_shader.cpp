#include "shader.h"

#include "device_context.h"
#include "logical_device.h"
#include "vulkan_helper_funcs.h"

using namespace rend;

VulkanShader::VulkanShader(DeviceContext& context)
    : _context(context),
      _vk_module(VK_NULL_HANDLE)
{
}

VulkanShader::~VulkanShader(void)
{
    _context.get_device()->destroy_shader_module(_vk_module);
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

    VkShaderModuleCreateInfo info = vulkan_helpers::gen_shader_module_create_info();
    info.codeSize = size_bytes;
    info.pCode = static_cast<const uint32_t*>(code);

    _vk_module = _context.get_device()->create_shader_module(info);
    if(_vk_module == VK_NULL_HANDLE)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}
