#include "vulkan_device_context.h"

#include "gpu_memory_interface.h"
#include "logical_device.h"
#include "physical_device.h"
#include "window.h"
#include "vulkan_helper_funcs.h"
#include "vulkan_instance.h"

#include <cassert>
#include <GLFW/glfw3.h>

using namespace rend;
using namespace rend::vkal;
using namespace rend::vkal::memory;

VulkanDeviceContext::VulkanDeviceContext(void)
{
    assert(_service == nullptr);
    _service = this;
}

VulkanDeviceContext::~VulkanDeviceContext(void)
{
    destroy();
}

PhysicalDevice* VulkanDeviceContext::gpu(void) const
{
    return _chosen_gpu;
}

GPUMemoryInterface* VulkanDeviceContext::memory_interface(void) const
{
    return _memory_interface;
}

LogicalDevice* VulkanDeviceContext::get_device(void) const
{
    return _logical_device;
}

StatusCode VulkanDeviceContext::create(void)
{
    // Create physical devices
    std::vector<VkPhysicalDevice> physical_devices;
    VulkanInstance::instance().enumerate_physical_devices(physical_devices);


    for(size_t physical_device_index = 0; physical_device_index < physical_devices.size(); physical_device_index++)
    {
        PhysicalDevice* pdev = new PhysicalDevice;
        pdev->create_physical_device(physical_device_index, physical_devices[physical_device_index]);
        _physical_devices.push_back(pdev);
    }

    return StatusCode::SUCCESS;
}

void VulkanDeviceContext::destroy(void)
{
    for (auto& handle : _vk_buffers)
    {
        if (_vk_buffers.check_valid(handle))
        {
            _logical_device->destroy_buffer(*_vk_buffers.get(handle));
        }
    }

    for(size_t physical_device_index = 0; physical_device_index < _physical_devices.size(); physical_device_index++)
    {
        delete _physical_devices[physical_device_index];
    }

    _logical_device = nullptr;
    _chosen_gpu     = nullptr;
}

StatusCode VulkanDeviceContext::choose_gpu(const VkPhysicalDeviceFeatures& desired_features)
{
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

StatusCode VulkanDeviceContext::create_device(const VkQueueFlags desired_queues)
{
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

VertexBufferHandle VulkanDeviceContext::create_vertex_buffer(uint32_t vertices_count, size_t vertex_size)
{
    return static_cast<IndexBufferHandle>(_create_buffer_internal(
        vertices_count * vertex_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    ));
}

IndexBufferHandle VulkanDeviceContext::create_index_buffer(uint32_t indices_count, size_t index_size)
{
    return static_cast<IndexBufferHandle>(_create_buffer_internal(
        indices_count * index_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    ));
}

UniformBufferHandle VulkanDeviceContext::create_uniform_buffer(size_t bytes)
{
    return static_cast<UniformBufferHandle>(_create_buffer_internal(
        bytes,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    ));
}

void VulkanDeviceContext::destroy_buffer(BufferHandle handle)
{
    MemoryHandle mem_handle = _buffer_to_memory[handle];
    VkBuffer* buffer = _vk_buffers.get(handle);
    VkDeviceMemory* memory = _vk_memorys.get(mem_handle);

    if (buffer)
    {
        _logical_device->destroy_buffer(*buffer);
        _vk_buffers.deallocate(handle);
    }

    if (memory)
    {
        // TODO: Eventually I want to conserve memory usage and pack buffers
        //       into memory blocks. So this will have to be removed.
        _logical_device->free_memory(*memory);
        _vk_memorys.deallocate(mem_handle);
    }

    _buffer_to_memory[handle] = NULL_HANDLE;
}

VkBuffer VulkanDeviceContext::get_buffer(VertexBufferHandle handle) const
{
    return *_vk_buffers.get(handle);
}

VkDeviceMemory VulkanDeviceContext::get_memory(BufferHandle vb_handle) const
{
    MemoryHandle mem_handle = _buffer_to_memory.at(vb_handle);
    return *_vk_memorys.get(mem_handle);
}

PhysicalDevice* VulkanDeviceContext::_find_physical_device(const VkPhysicalDeviceFeatures& features)
{
    for(PhysicalDevice* device : _physical_devices)
    {
        if(device->has_features(features))
            return device;
    }

    return nullptr;
}

BufferHandle VulkanDeviceContext::_create_buffer_internal(size_t bytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties)
{
    uint32_t queue_family_index = _logical_device->get_queue_family(QueueType::GRAPHICS)->get_index();

    VkBufferCreateInfo create_info    = vulkan_helpers::gen_buffer_create_info();
    create_info.size                  = bytes;
    create_info.usage                 = usage;
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices   = &queue_family_index;

    VkBuffer buffer = _logical_device->create_buffer(create_info);

    VkMemoryRequirements memory_reqs = _logical_device->get_buffer_memory_reqs(buffer);

    VkMemoryAllocateInfo alloc_info = vulkan_helpers::gen_memory_allocate_info();
    alloc_info.allocationSize = bytes;
    alloc_info.memoryTypeIndex = _logical_device->find_memory_type(memory_reqs.memoryTypeBits, memory_properties);

    VkDeviceMemory memory = _logical_device->allocate_memory(alloc_info);

    _logical_device->bind_buffer_memory(buffer, memory);

    BufferHandle handle = _vk_buffers.allocate(buffer);
    MemoryHandle mem_handle = _vk_memorys.allocate(memory);

    _buffer_to_memory[handle] = mem_handle;

    return handle;

}
