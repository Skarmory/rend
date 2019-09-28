#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H

#include "queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandBuffer;
class CommandPool;
class DescriptorPool;
class DescriptorSetLayout;
class DeviceContext;
class Event;
class Fence;
class Framebuffer;
class GPUBuffer;
class Image;
class PhysicalDevice;
class Pipeline;
class PipelineLayout;
class RenderPass;
class Semaphore;
class Shader;
class Swapchain;

struct PipelineSettings;

enum class ShaderType;

class LogicalDevice
{

public:
    LogicalDevice(const DeviceContext* device_context, const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family);
    ~LogicalDevice(void);

    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&&) = delete;

    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

    bool create_logical_device(void);

    // Retrieval
    const DeviceContext&  get_device_context(void) const;
    const PhysicalDevice& get_physical_device(void) const;
    VkDevice              get_handle(void) const;
    VkQueue               get_queue(QueueType type) const;
    const QueueFamily*    get_queue_family(QueueType type) const;

    // Commands
    bool                  queue_submit(const std::vector<CommandBuffer*>& command_buffers, QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Semaphore*>& signal_sems, Fence* fence);
    uint32_t              find_memory_type(uint32_t desired_type, VkMemoryPropertyFlags memory_properties);

    // Creational
    Pipeline*             create_pipeline(PipelineSettings* settings);
    void                  destroy_pipeline(Pipeline** pipeline);

    Shader*               create_shader(const void* data, uint32_t size_bytes, ShaderType type);
    void                  destroy_shader(Shader** shader);

private:
    VkDevice _vk_device;
    VkQueue  _vk_graphics_queue;
    VkQueue  _vk_transfer_queue;

    const DeviceContext*  _context;
    const PhysicalDevice* _physical_device;
    const QueueFamily*    _graphics_family;
    const QueueFamily*    _transfer_family;

    std::vector<CommandPool*> _command_pools;
};

}

#endif
