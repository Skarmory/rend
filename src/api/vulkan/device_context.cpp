#include "device_context.h"

#include "gpu_memory_interface.h"
#include "physical_device.h"
#include "window.h"
#include "vulkan_instance.h"

#include <cassert>
#include <GLFW/glfw3.h>

using namespace rend;
using namespace rend::core;
using namespace rend::vkal;
using namespace rend::vkal::memory;

DeviceContext::~DeviceContext(void)
{
    destroy();
}

DeviceContext& DeviceContext::instance(void)
{
    static DeviceContext s_context;

    return s_context;
}

PhysicalDevice* DeviceContext::gpu(void) const
{
    return _chosen_gpu;
}

GPUMemoryInterface* DeviceContext::memory_interface(void) const
{
    return _memory_interface;
}

LogicalDevice* DeviceContext::get_device(void) const
{
    return _logical_device;
}

StatusCode DeviceContext::create(void)
{
    assert(!initialised());

    if(initialised())
    {
        return StatusCode::ALREADY_CREATED;
    }

    // Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    VulkanInstance::instance().enumerate_physical_devices(physical_devices);


    for(size_t physical_device_index = 0; physical_device_index < physical_devices.size(); physical_device_index++)
    {
        PhysicalDevice* pdev = new PhysicalDevice;
        pdev->create_physical_device(physical_device_index, physical_devices[physical_device_index]);
        _physical_devices.push_back(pdev);
    }

    create_resource();

    return StatusCode::SUCCESS;
}

void DeviceContext::destroy(void)
{
    assert(initialised() == true);

    for(size_t physical_device_index = 0; physical_device_index < _physical_devices.size(); physical_device_index++)
    {
        delete _physical_devices[physical_device_index];
    }

    _logical_device = nullptr;
    _chosen_gpu     = nullptr;

    destroy_resource();
}

StatusCode DeviceContext::choose_gpu(const VkPhysicalDeviceFeatures& desired_features)
{
    assert(initialised() == true);

    if(_chosen_gpu)
    {
        return StatusCode::ALREADY_CREATED;
    }

    _chosen_gpu = _find_physical_device(desired_features);

    if(!_chosen_gpu)
    {
        return StatusCode::CONTEXT_GPU_WITH_DESIRED_FEATURES_NOT_FOUND;
    }

    return StatusCode::SUCCESS;
}

StatusCode DeviceContext::create_device(const VkQueueFlags desired_queues)
{
    assert(initialised() == true);

    if(_logical_device)
    {
        return StatusCode::ALREADY_CREATED;
    }

    if(!_chosen_gpu)
    {
        return StatusCode::CONTEXT_GPU_NOT_CHOSEN;
    }

    if(!_chosen_gpu->create_logical_device(desired_queues))
    {
        return StatusCode::CONTEXT_DEVICE_CREATE_FAILURE;
    }

    _memory_interface = new GPUMemoryInterface;
    _memory_interface->create(*_chosen_gpu);
    _logical_device = _chosen_gpu->get_logical_device();

    return StatusCode::SUCCESS;
}

PhysicalDevice* DeviceContext::_find_physical_device(const VkPhysicalDeviceFeatures& features)
{
    for(PhysicalDevice* device : _physical_devices)
    {
        if(device->has_features(features))
            return device;
    }

    return nullptr;
}
