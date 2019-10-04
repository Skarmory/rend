#ifndef REND_LOGICAL_DEVICE_H
#define REND_LOGICAL_DEVICE_H

#include "queue_family.h"

#include <vulkan.h>
#include <vector>

namespace rend
{

class CommandBuffer;
class DeviceContext;
class Fence;
class PhysicalDevice;
class Semaphore;
class Swapchain;
struct PipelineSettings;
enum class ShaderType;

class LogicalDevice
{

public:
    LogicalDevice(const DeviceContext& device_context);
    ~LogicalDevice(void);

    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&&) = delete;

    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

    bool create_logical_device(const PhysicalDevice* physical_device, const QueueFamily* const graphics_family, const QueueFamily* const transfer_family);

    // Retrieval
    const DeviceContext&  get_device_context(void) const;
    const PhysicalDevice& get_physical_device(void) const;
    VkDevice              get_handle(void) const;
    VkQueue               get_queue(QueueType type) const;
    const QueueFamily*    get_queue_family(QueueType type) const;

    // Commands
    bool                  queue_submit(const std::vector<CommandBuffer*>& command_buffers, QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Semaphore*>& signal_sems, Fence* fence);
    uint32_t              find_memory_type(uint32_t desired_type, VkMemoryPropertyFlags memory_properties);
    void                  wait_idle(void);
    VkResult              acquire_next_image(Swapchain* swapchain, uint64_t timeout, Semaphore* semaphore, Fence* fence, uint32_t* image_index);
    VkResult              queue_present(QueueType type, const std::vector<Semaphore*>& wait_sems, const std::vector<Swapchain*>& swapchains, const std::vector<uint32_t>& image_indices, std::vector<VkResult>& results);
    VkResult              get_swapchain_images(Swapchain* swapchain, std::vector<VkImage>& images);
    VkMemoryRequirements  get_buffer_memory_reqs(VkBuffer buffer);
    VkResult              bind_buffer_memory(VkBuffer buffer, VkDeviceMemory memory);

    VkDeviceMemory allocate_memory(VkDeviceSize size_bytes, VkMemoryRequirements reqs, VkMemoryPropertyFlags props);
    void           free_memory(VkDeviceMemory memory);

    VkSwapchainKHR create_swapchain(
        VkSurfaceKHR surface, uint32_t min_image_count, VkFormat format,
        VkColorSpaceKHR colour_space, VkExtent2D extent, uint32_t array_layers,
        VkImageUsageFlags image_usage, VkSharingMode sharing_mode, uint32_t queue_family_index_count,
        const uint32_t* queue_family_indices, VkSurfaceTransformFlagBitsKHR pre_transform, VkCompositeAlphaFlagBitsKHR composite_alpha,
        VkPresentModeKHR present_mode, VkBool32 clipped, VkSwapchainKHR old_swapchain
    );

    void destroy_swapchain(VkSwapchainKHR swapchain);

    VkImageView create_image_view(
        VkImage image, VkImageViewType viewType, VkFormat format,
        VkComponentMapping components, VkImageSubresourceRange subresourceRange
    );

    void destroy_image_view(VkImageView image_view);

    VkBuffer create_buffer(
        VkDeviceSize size_bytes, VkBufferUsageFlags usage, VkSharingMode sharing_mode,
        uint32_t queue_family_index_count, uint32_t* queue_family_indices
    );

    void destroy_buffer(VkBuffer buffer);

private:
    const DeviceContext&  _context;
    const PhysicalDevice* _physical_device;
    const QueueFamily*    _graphics_family;
    const QueueFamily*    _transfer_family;

    VkDevice _vk_device;
    VkQueue  _vk_graphics_queue;
    VkQueue  _vk_transfer_queue;
};

}

#endif
