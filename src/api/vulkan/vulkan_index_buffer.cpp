#include "vulkan_index_buffer.h"

#include "device_context.h"
#include "vulkan_gpu_buffer.h"
#include "logical_device.h"

using namespace rend;

VulkanIndexBuffer::VulkanIndexBuffer(DeviceContext& context)
    : VulkanGPUBuffer(context)
{
}

VulkanIndexBuffer::~VulkanIndexBuffer(void)
{
}

StatusCode VulkanIndexBuffer::create_index_buffer_api(uint32_t indices_count, size_t index_size)
{
    StatusCode code = create_buffer(indices_count * index_size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    if(code == StatusCode::SUCCESS)
    {
        _count = indices_count;
    }

    return code;
}
