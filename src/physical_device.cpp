#include "physical_device.h"

#include "device_context.h"
#include "logical_device.h"
#include "utils.h"

#include <iostream>

using namespace rend;

PhysicalDevice::PhysicalDevice(const DeviceContext* context, uint32_t physical_device_index, VkPhysicalDevice physical_device) : _physical_device_index(physical_device_index), _vk_physical_device(physical_device), _context(context), _logical_device(nullptr)
{
    std::cout << "Constructing physical device" << std::endl;

    vkGetPhysicalDeviceProperties(_vk_physical_device, &_vk_physical_device_properties);
    vkGetPhysicalDeviceFeatures(_vk_physical_device, &_vk_physical_device_features);

    VkSurfaceKHR surface = _context->get_surface();
    _find_queue_families(surface);
    _find_surface_formats(surface);
    _find_surface_present_modes(surface);
}

PhysicalDevice::~PhysicalDevice(void)
{
    std::cout << "Destructing physical device" << std::endl;

    if(_logical_device)
        delete _logical_device;
}

void PhysicalDevice::_find_queue_families(VkSurfaceKHR surface)
{
    uint32_t count;
    std::vector<VkQueueFamilyProperties> queue_family_properties;
    vkGetPhysicalDeviceQueueFamilyProperties(_vk_physical_device, &count, nullptr);

    std::cout << "Found " << count << " queue families" << std::endl;

    _queue_families.reserve(count);
    queue_family_properties.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(_vk_physical_device, &count, queue_family_properties.data());

    for(size_t queue_family_index = 0; queue_family_index < queue_family_properties.size(); queue_family_index++)
    {
        VkBool32 supports_present;
        vkGetPhysicalDeviceSurfaceSupportKHR(_vk_physical_device, queue_family_index, surface, &supports_present);
       _queue_families.push_back(QueueFamily(queue_family_index, queue_family_properties[queue_family_index], supports_present));

       QueueFamily* queue_family = &_queue_families[queue_family_index];

       if(queue_family->get_properties().queueFlags & VK_QUEUE_GRAPHICS_BIT)
           _graphics_queue_families.push_back(queue_family);

       if(queue_family->supports_present_queue())
           _present_queue_families.push_back(queue_family);
    }
}

void PhysicalDevice::_find_surface_formats(VkSurfaceKHR surface)
{
    uint32_t count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_vk_physical_device, surface, &count, nullptr);

    if(count == 0)
        return;

   _vk_surface_formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(_vk_physical_device, surface, &count, _vk_surface_formats.data());
}

void PhysicalDevice::_find_surface_present_modes(VkSurfaceKHR surface)
{
    uint32_t count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_vk_physical_device, surface, &count, nullptr);

    if(count == 0)
        return;

    _vk_present_modes.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(_vk_physical_device, surface, &count, _vk_present_modes.data());
}

LogicalDevice* PhysicalDevice::create_logical_device(const VkQueueFlags queue_flags)
{
    if(_logical_device)
    {
        std::cerr << "Attempted to create logical device when one already exists" << std::endl;
        return _logical_device;
    }

    QueueFamily* graphics_family = nullptr;
    QueueFamily* present_family = nullptr;

    if(queue_flags & VK_QUEUE_GRAPHICS_BIT)
    {
        DEATH_CHECK(_graphics_queue_families.size() == 0, "Physical device does not support a graphics queue family");

        graphics_family = _graphics_queue_families[0];

        DEATH_CHECK(_present_queue_families.size() == 0, "Physical device does not support a present queue family");

        present_family = _present_queue_families[0];
    }

    _logical_device = new LogicalDevice(_context, this, graphics_family, present_family);

    return _logical_device;
}

uint32_t PhysicalDevice::get_index(void) const
{
    return _physical_device_index;
}

VkPhysicalDevice PhysicalDevice::get_handle(void) const
{
    return _vk_physical_device;
}

const std::vector<VkSurfaceFormatKHR>& PhysicalDevice::get_surface_formats(void) const
{
    return _vk_surface_formats;
}

const std::vector<VkPresentModeKHR>& PhysicalDevice::get_surface_present_modes(void) const
{
    return _vk_present_modes;
}

VkSurfaceCapabilitiesKHR PhysicalDevice::get_surface_capabilities(void) const
{
    VkSurfaceCapabilitiesKHR caps;
    VULKAN_DEATH_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_vk_physical_device, _context->get_surface(), &caps), "Failed to get surface capabilities");

    return caps;
}

bool PhysicalDevice::has_queues(VkQueueFlags queue_flags) const
{
    if(queue_flags & VK_QUEUE_GRAPHICS_BIT && _graphics_queue_families.size() == 0 && _present_queue_families.size() == 0)
        return false;

    return true;
}

bool PhysicalDevice::has_features(const VkPhysicalDeviceFeatures& features) const
{
   if(features.robustBufferAccess && !_vk_physical_device_features.robustBufferAccess)
       return false;
   if(features.fullDrawIndexUint32 && !_vk_physical_device_features.fullDrawIndexUint32)
       return false;
   if(features.imageCubeArray && !_vk_physical_device_features.imageCubeArray)
       return false;
   if(features.independentBlend && !_vk_physical_device_features.independentBlend)
       return false;
   if(features.geometryShader && !_vk_physical_device_features.geometryShader)
       return false;
   if(features.tessellationShader && !_vk_physical_device_features.tessellationShader)
       return false;
   if(features.sampleRateShading && !_vk_physical_device_features.sampleRateShading)
       return false;
   if(features.dualSrcBlend && !_vk_physical_device_features.dualSrcBlend)
       return false;
   if(features.logicOp && !_vk_physical_device_features.logicOp)
       return false;
   if(features.multiDrawIndirect && !_vk_physical_device_features.multiDrawIndirect)
       return false;
   if(features.drawIndirectFirstInstance && !_vk_physical_device_features.drawIndirectFirstInstance)
       return false;
   if(features.depthClamp && !_vk_physical_device_features.depthClamp)
       return false;
   if(features.depthBiasClamp && !_vk_physical_device_features.depthBiasClamp)
       return false;
   if(features.fillModeNonSolid && !_vk_physical_device_features.fillModeNonSolid)
       return false;
   if(features.depthBounds && !_vk_physical_device_features.depthBounds)
       return false;
   if(features.wideLines && !_vk_physical_device_features.wideLines)
       return false;
   if(features.largePoints && !_vk_physical_device_features.largePoints)
       return false;
   if(features.alphaToOne && !_vk_physical_device_features.alphaToOne)
       return false;
   if(features.multiViewport && !_vk_physical_device_features.multiViewport)
       return false;
   if(features.samplerAnisotropy && !_vk_physical_device_features.samplerAnisotropy)
       return false;
   if(features.textureCompressionETC2 && !_vk_physical_device_features.textureCompressionETC2)
       return false;
   if(features.textureCompressionASTC_LDR && !_vk_physical_device_features.textureCompressionASTC_LDR)
       return false;
   if(features.textureCompressionBC && !_vk_physical_device_features.textureCompressionBC)
       return false;
   if(features.occlusionQueryPrecise && !_vk_physical_device_features.occlusionQueryPrecise)
       return false;
   if(features.pipelineStatisticsQuery && !_vk_physical_device_features.pipelineStatisticsQuery)
       return false;
   if(features.vertexPipelineStoresAndAtomics && !_vk_physical_device_features.vertexPipelineStoresAndAtomics)
       return false;
   if(features.fragmentStoresAndAtomics && !_vk_physical_device_features.fragmentStoresAndAtomics)
       return false;
   if(features.shaderTessellationAndGeometryPointSize && !_vk_physical_device_features.shaderTessellationAndGeometryPointSize)
       return false;
   if(features.shaderImageGatherExtended && !_vk_physical_device_features.shaderImageGatherExtended)
       return false;
   if(features.shaderStorageImageExtendedFormats && !_vk_physical_device_features.shaderStorageImageExtendedFormats)
       return false;
   if(features.shaderStorageImageMultisample && !_vk_physical_device_features.shaderStorageImageMultisample)
       return false;
   if(features.shaderStorageImageReadWithoutFormat && !_vk_physical_device_features.shaderStorageImageReadWithoutFormat)
       return false;
   if(features.shaderStorageImageWriteWithoutFormat && !_vk_physical_device_features.shaderStorageImageWriteWithoutFormat)
       return false;
   if(features.shaderUniformBufferArrayDynamicIndexing && !_vk_physical_device_features.shaderUniformBufferArrayDynamicIndexing)
       return false;
   if(features.shaderSampledImageArrayDynamicIndexing && !_vk_physical_device_features.shaderSampledImageArrayDynamicIndexing)
       return false;
   if(features.shaderStorageBufferArrayDynamicIndexing && !_vk_physical_device_features.shaderStorageBufferArrayDynamicIndexing)
       return false;
   if(features.shaderStorageImageArrayDynamicIndexing && !_vk_physical_device_features.shaderStorageImageArrayDynamicIndexing)
       return false;
   if(features.shaderClipDistance && !_vk_physical_device_features.shaderClipDistance)
       return false;
   if(features.shaderCullDistance && !_vk_physical_device_features.shaderCullDistance)
       return false;
   if(features.shaderFloat64 && !_vk_physical_device_features.shaderFloat64)
       return false;
   if(features.shaderInt64 && !_vk_physical_device_features.shaderInt64)
       return false;
   if(features.shaderInt16 && !_vk_physical_device_features.shaderInt16)
       return false;
   if(features.shaderResourceResidency && !_vk_physical_device_features.shaderResourceResidency)
       return false;
   if(features.shaderResourceMinLod && !_vk_physical_device_features.shaderResourceMinLod)
       return false;
   if(features.sparseBinding && !_vk_physical_device_features.sparseBinding)
       return false;
   if(features.sparseResidencyBuffer && !_vk_physical_device_features.sparseResidencyBuffer)
       return false;
   if(features.sparseResidencyImage2D && !_vk_physical_device_features.sparseResidencyImage2D)
       return false;
   if(features.sparseResidencyImage3D && !_vk_physical_device_features.sparseResidencyImage3D)
       return false;
   if(features.sparseResidency2Samples && !_vk_physical_device_features.sparseResidency2Samples)
       return false;
   if(features.sparseResidency4Samples && !_vk_physical_device_features.sparseResidency4Samples)
       return false;
   if(features.sparseResidency8Samples && !_vk_physical_device_features.sparseResidency8Samples)
       return false;
   if(features.sparseResidency16Samples && !_vk_physical_device_features.sparseResidency16Samples)
       return false;
   if(features.sparseResidencyAliased && !_vk_physical_device_features.sparseResidencyAliased)
       return false;
   if(features.variableMultisampleRate && !_vk_physical_device_features.variableMultisampleRate)
       return false;
   if(features.inheritedQueries && !_vk_physical_device_features.inheritedQueries)
       return false;

   return true;
}
