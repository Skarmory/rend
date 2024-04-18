#include "api/vulkan/vulkan_buffer.h"

using namespace rend;

VulkanBuffer::VulkanBuffer(const std::string& name, RendHandle rend_handle, const BufferInfo& info, const VulkanBufferInfo& vk_buffer_info)
    :
        GPUBuffer(name, rend_handle, info),
        _vk_buffer_info(vk_buffer_info)
{
}

const VulkanBufferInfo& VulkanBuffer::vk_buffer_info(void) const
{
    return _vk_buffer_info;
}
