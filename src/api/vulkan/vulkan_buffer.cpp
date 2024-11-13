#include "api/vulkan/vulkan_buffer.h"

using namespace rend;

VulkanBuffer::VulkanBuffer(const std::string& name, const BufferInfo& info, const VulkanBufferInfo& vk_buffer_info)
    :
        GPUBuffer(name, info),
        _vk_buffer_info(vk_buffer_info)
{
}

const VulkanBufferInfo& VulkanBuffer::vk_buffer_info(void) const
{
    return _vk_buffer_info;
}
