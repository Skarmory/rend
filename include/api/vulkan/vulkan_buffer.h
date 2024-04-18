#ifndef REND_API_VULKAN_VULKAN_BUFFER_H
#define REND_API_VULKAN_VULKAN_BUFFER_H

#include "api/vulkan/vulkan_buffer_info.h"
#include "core/gpu_buffer.h"

namespace rend
{

struct VulkanBufferInfo;

class VulkanBuffer : public GPUBuffer
{
    public:
        VulkanBuffer(const std::string& name, RendHandle rend_handle, const BufferInfo& info, const VulkanBufferInfo& vk_buffer_info);
        ~VulkanBuffer(void) = default;

        const VulkanBufferInfo& vk_buffer_info(void) const;

    private:
        VulkanBufferInfo _vk_buffer_info{};
};

}

#endif
