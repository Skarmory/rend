#ifndef REND_DEVICE_CONTEXT_H
#define REND_DEVICE_CONTEXT_H

#include "data_array.h"
#include "resource.h"

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

typedef DataArrayHandle BufferHandle;

class DeviceContext : public core::Resource
{
public:
    DeviceContext(const DeviceContext&)            = delete;
    DeviceContext(DeviceContext&&)                 = delete;
    DeviceContext& operator=(const DeviceContext&) = delete;
    DeviceContext& operator=(DeviceContext&&)      = delete;

    static DeviceContext& instance(void);

    PhysicalDevice*                         gpu(void) const;
    rend::vkal::memory::GPUMemoryInterface* memory_interface(void) const;
    LogicalDevice*                          get_device(void) const;

    StatusCode create(void);
    void       destroy(void);

    StatusCode choose_gpu(const VkPhysicalDeviceFeatures& desired_features);
    StatusCode create_device(const VkQueueFlags desired_queues);

    BufferHandle create_buffer(VkBufferCreateInfo& create_info);

private:
    DeviceContext(void) = default;
    ~DeviceContext(void);

    PhysicalDevice* _find_physical_device(const VkPhysicalDeviceFeatures& features);

private:
    std::vector<PhysicalDevice*>             _physical_devices;
    rend::vkal::memory::GPUMemoryInterface*  _memory_interface  { nullptr };
    LogicalDevice*                           _logical_device    { nullptr };
    PhysicalDevice*                          _chosen_gpu        { nullptr };

    DataArray<VkBuffer> _vk_buffers;
};

}

#endif
