#ifndef REND_VULKAN_DEVICE_CONTEXT_H
#define REND_VULKAN_DEVICE_CONTEXT_H

#include "core/data_structures/data_array.h"
#include "device_context.h"
#include "rend_defs.h"
#include "resource.h"

#include <unordered_map>
#include <vulkan.h>
#include <vector>

namespace rend::vkal::memory
{
	class GPUMemoryInterface;
}

namespace rend
{

class PhysicalDevice;
class LogicalDevice;

class VulkanDeviceContext : public DeviceContext
{
public:
    VulkanDeviceContext(void);
    ~VulkanDeviceContext(void);
    VulkanDeviceContext(const VulkanDeviceContext&)            = delete;
    VulkanDeviceContext(VulkanDeviceContext&&)                 = delete;
    VulkanDeviceContext& operator=(const VulkanDeviceContext&) = delete;
    VulkanDeviceContext& operator=(VulkanDeviceContext&&)      = delete;

    PhysicalDevice*                         gpu(void) const;
    rend::vkal::memory::GPUMemoryInterface* memory_interface(void) const;
    LogicalDevice*                          get_device(void) const;

    StatusCode create(void) override;
    void       destroy(void) override;

    StatusCode choose_gpu(const VkPhysicalDeviceFeatures& desired_features);
    StatusCode create_device(const VkQueueFlags desired_queues);

    VertexBufferHandle create_vertex_buffer(uint32_t vertices_count, size_t vertex_size) override;

    VkBuffer get_buffer(VertexBufferHandle handle) const;
    VkDeviceMemory get_memory(VertexBufferHandle handle) const;

private:
    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

private:
    std::vector<PhysicalDevice*>             _physical_devices;
    rend::vkal::memory::GPUMemoryInterface*  _memory_interface  { nullptr };
    LogicalDevice*                           _logical_device    { nullptr };
    PhysicalDevice*                          _chosen_gpu        { nullptr };

    DataArray<VkBuffer> _vk_buffers;
    DataArray<VkDeviceMemory> _vk_memorys;
    std::unordered_map<VertexBufferHandle, MemoryHandle> _vb_to_memory;
};

}

#endif
