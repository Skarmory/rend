//#ifndef REND_PI_VULKAN_VULKAN_COMMAND_POOL_H
//#define REND_PI_VULKAN_VULKAN_COMMAND_POOL_H
//
//#include "core/command_buffer.h"
//#include "core/containers/data_array.h"
//
//namespace rend
//{
//
//class VulkanCommandPool
//{
//    public:
//        CommandPool(void);
//        ~CommandPool(void);
//        CommandPool(const CommandPool&)            = delete;
//        CommandPool(CommandPool&&)                 = delete;
//        CommandPool& operator=(const CommandPool&) = delete;
//        CommandPool& operator=(CommandPool&&)      = delete;
//
//        VkCommandPool handle(void) const;
//
//        CommandBuffer* create_command_buffer(void);
//        void           destroy_command_buffer(CommandBuffer* command_buffer);
//        void           reset(void);
//
//    private:
//        VkCommandPool _handle{ VK_NULL_HANDLE };
//};
//
//}
//
//#endif
