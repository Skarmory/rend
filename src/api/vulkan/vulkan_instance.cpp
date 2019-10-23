#include "vulkan_instance.h"

using namespace rend;

VulkanInstance::VulkanInstance(void)
    : _vk_instance(VK_NULL_HANDLE)
{
}

VulkanInstance::~VulkanInstance(void)
{
    vkDestroyInstance(_vk_instance, nullptr);
}

StatusCode VulkanInstance::create_instance(const char** extensions, uint32_t extension_count, const char** layers, uint32_t layer_count)
{
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = 1,
        .pEngineName = "Rend",
        .engineVersion = 1,
        .apiVersion = VK_API_VERSION_1_1
    };

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions
    };

    if(vkCreateInstance(&instance_create_info, nullptr, &_vk_instance) != VK_SUCCESS)
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

VkInstance VulkanInstance::get_handle(void) const
{
    return _vk_instance;
}