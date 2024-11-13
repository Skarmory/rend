#include "api/vulkan/physical_device.h"

#include "core/device_context.h"
#include "core/rend_service.h"
#include "core/window.h"

#include "api/vulkan/logical_device.h"
#include "api/vulkan/device_features.h"
#include "api/vulkan/vulkan_instance.h"

#include <cassert>

using namespace rend;

PhysicalDevice::PhysicalDevice(const VulkanInstance& vk_instance, uint32_t physical_device_index, VkPhysicalDevice physical_device)
{
    _physical_device_index = physical_device_index;
    _vk_physical_device = physical_device;

    vkGetPhysicalDeviceProperties(_vk_physical_device, &_vk_physical_device_properties);
    vkGetPhysicalDeviceMemoryProperties(_vk_physical_device, &_vk_physical_device_memory_properties);

    VkSurfaceKHR surface = vk_instance.surface();
    _find_queue_families(surface);
    _find_surface_formats(surface);
    _find_surface_present_modes(surface);
}

PhysicalDevice::~PhysicalDevice(void)
{
}

LogicalDevice* PhysicalDevice::create_logical_device(const VkQueueFlags queue_flags, const std::vector<DeviceFeature>& desired_features)
{
    QueueFamily* graphics_family = nullptr;
    QueueFamily* present_family = nullptr;

    if(queue_flags & VK_QUEUE_GRAPHICS_BIT)
    {
        if(_graphics_queue_families.empty() || _present_queue_families.empty())
        {
            return nullptr;
        }

        graphics_family = _graphics_queue_families[0];
        present_family  = _present_queue_families[0];
    }

    LogicalDevice* logical_device = new LogicalDevice(this, graphics_family, present_family, desired_features);

    return logical_device;
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

VkSurfaceCapabilitiesKHR PhysicalDevice::get_surface_capabilities(const VulkanInstance& vk_instance) const
{
    VkSurfaceCapabilitiesKHR caps{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_vk_physical_device, vk_instance.surface(), &caps);

    return caps;
}
const VkPhysicalDeviceMemoryProperties& PhysicalDevice::get_memory_properties(void) const
{
    return _vk_physical_device_memory_properties;
}

bool PhysicalDevice::has_queues(VkQueueFlags queue_flags) const
{
    const bool has_graphics_queue = (queue_flags & VK_QUEUE_GRAPHICS_BIT) ? !_graphics_queue_families.empty()  : true;
    const bool has_present_queue  = !_present_queue_families.empty();

    return has_graphics_queue && has_present_queue;
}

bool PhysicalDevice::has_features(const std::vector<DeviceFeature>& desired_features) const
{
    PhysicalDeviceFeatures desired_features_built = PhysicalDeviceFeatures::make_device_features(desired_features);

    PhysicalDeviceFeatures check_features = PhysicalDeviceFeatures::make_device_features({});
    check_features.vk_1_1_features.pNext = &check_features.vk_1_2_features;
    check_features.vk_1_2_features.pNext = nullptr;

    VkPhysicalDeviceFeatures2 device_features;
    device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    device_features.pNext = &check_features.vk_1_1_features;
    device_features.features = check_features.vk_1_0_features;

    vkGetPhysicalDeviceFeatures2(_vk_physical_device, &device_features);

    check_features.vk_1_0_features = device_features.features;

    if(!check_features.vk_1_0_features.robustBufferAccess && desired_features_built.vk_1_0_features.robustBufferAccess)
       return false;
    if(!check_features.vk_1_0_features.fullDrawIndexUint32 && desired_features_built.vk_1_0_features.fullDrawIndexUint32)
       return false;
    if(!check_features.vk_1_0_features.imageCubeArray && desired_features_built.vk_1_0_features.imageCubeArray)
       return false;
    if(!check_features.vk_1_0_features.independentBlend && desired_features_built.vk_1_0_features.independentBlend)
       return false;
    if(!check_features.vk_1_0_features.geometryShader && desired_features_built.vk_1_0_features.geometryShader)
       return false;
    if(!check_features.vk_1_0_features.tessellationShader && desired_features_built.vk_1_0_features.tessellationShader)
       return false;
    if(!check_features.vk_1_0_features.sampleRateShading && desired_features_built.vk_1_0_features.sampleRateShading)
       return false;
    if(!check_features.vk_1_0_features.dualSrcBlend && desired_features_built.vk_1_0_features.dualSrcBlend)
       return false;
    if(!check_features.vk_1_0_features.logicOp && desired_features_built.vk_1_0_features.logicOp)
       return false;
    if(!check_features.vk_1_0_features.multiDrawIndirect && desired_features_built.vk_1_0_features.multiDrawIndirect)
       return false;
    if(!check_features.vk_1_0_features.drawIndirectFirstInstance && desired_features_built.vk_1_0_features.drawIndirectFirstInstance)
       return false;
    if(!check_features.vk_1_0_features.depthClamp && desired_features_built.vk_1_0_features.depthClamp)
       return false;
    if(!check_features.vk_1_0_features.depthBiasClamp && desired_features_built.vk_1_0_features.depthBiasClamp)
       return false;
    if(!check_features.vk_1_0_features.fillModeNonSolid && desired_features_built.vk_1_0_features.fillModeNonSolid)
       return false;
    if(!check_features.vk_1_0_features.depthBounds && desired_features_built.vk_1_0_features.depthBounds)
       return false;
    if(!check_features.vk_1_0_features.wideLines && desired_features_built.vk_1_0_features.wideLines)
       return false;
    if(!check_features.vk_1_0_features.largePoints && desired_features_built.vk_1_0_features.largePoints)
       return false;
    if(!check_features.vk_1_0_features.alphaToOne && desired_features_built.vk_1_0_features.alphaToOne)
       return false;
    if(!check_features.vk_1_0_features.multiViewport && desired_features_built.vk_1_0_features.multiViewport)
       return false;
    if(!check_features.vk_1_0_features.samplerAnisotropy && desired_features_built.vk_1_0_features.samplerAnisotropy)
       return false;
    if(!check_features.vk_1_0_features.textureCompressionETC2 && desired_features_built.vk_1_0_features.textureCompressionETC2)
       return false;
    if(!check_features.vk_1_0_features.textureCompressionASTC_LDR && desired_features_built.vk_1_0_features.textureCompressionASTC_LDR)
       return false;
    if(!check_features.vk_1_0_features.textureCompressionBC && desired_features_built.vk_1_0_features.textureCompressionBC)
       return false;
    if(!check_features.vk_1_0_features.occlusionQueryPrecise && desired_features_built.vk_1_0_features.occlusionQueryPrecise)
       return false;
    if(!check_features.vk_1_0_features.pipelineStatisticsQuery && desired_features_built.vk_1_0_features.pipelineStatisticsQuery)
       return false;
    if(!check_features.vk_1_0_features.vertexPipelineStoresAndAtomics && desired_features_built.vk_1_0_features.vertexPipelineStoresAndAtomics)
       return false;
    if(!check_features.vk_1_0_features.fragmentStoresAndAtomics && desired_features_built.vk_1_0_features.fragmentStoresAndAtomics)
       return false;
    if(!check_features.vk_1_0_features.shaderTessellationAndGeometryPointSize && desired_features_built.vk_1_0_features.shaderTessellationAndGeometryPointSize)
       return false;
    if(!check_features.vk_1_0_features.shaderImageGatherExtended && desired_features_built.vk_1_0_features.shaderImageGatherExtended)
       return false;
    if(!check_features.vk_1_0_features.shaderStorageImageExtendedFormats && desired_features_built.vk_1_0_features.shaderStorageImageExtendedFormats)
       return false;
    if(!check_features.vk_1_0_features.shaderStorageImageMultisample && desired_features_built.vk_1_0_features.shaderStorageImageMultisample)
       return false;
    if(!check_features.vk_1_0_features.shaderStorageImageReadWithoutFormat && desired_features_built.vk_1_0_features.shaderStorageImageReadWithoutFormat)
       return false;
    if(!check_features.vk_1_0_features.shaderStorageImageWriteWithoutFormat && desired_features_built.vk_1_0_features.shaderStorageImageWriteWithoutFormat)
       return false;
    if(!check_features.vk_1_0_features.shaderUniformBufferArrayDynamicIndexing && desired_features_built.vk_1_0_features.shaderUniformBufferArrayDynamicIndexing)
       return false;
    if(!check_features.vk_1_0_features.shaderSampledImageArrayDynamicIndexing && desired_features_built.vk_1_0_features.shaderSampledImageArrayDynamicIndexing)
       return false;
    if(!check_features.vk_1_0_features.shaderStorageBufferArrayDynamicIndexing && desired_features_built.vk_1_0_features.shaderStorageBufferArrayDynamicIndexing)
       return false;
    if(!check_features.vk_1_0_features.shaderStorageImageArrayDynamicIndexing && desired_features_built.vk_1_0_features.shaderStorageImageArrayDynamicIndexing)
       return false;
    if(!check_features.vk_1_0_features.shaderClipDistance && desired_features_built.vk_1_0_features.shaderClipDistance)
       return false;
    if(!check_features.vk_1_0_features.shaderCullDistance && desired_features_built.vk_1_0_features.shaderCullDistance)
       return false;
    if(!check_features.vk_1_0_features.shaderFloat64 && desired_features_built.vk_1_0_features.shaderFloat64)
       return false;
    if(!check_features.vk_1_0_features.shaderInt64 && desired_features_built.vk_1_0_features.shaderInt64)
       return false;
    if(!check_features.vk_1_0_features.shaderInt16 && desired_features_built.vk_1_0_features.shaderInt16)
       return false;
    if(!check_features.vk_1_0_features.shaderResourceResidency && desired_features_built.vk_1_0_features.shaderResourceResidency)
       return false;
    if(!check_features.vk_1_0_features.shaderResourceMinLod && desired_features_built.vk_1_0_features.shaderResourceMinLod)
       return false;
    if(!check_features.vk_1_0_features.sparseBinding && desired_features_built.vk_1_0_features.sparseBinding)
       return false;
    if(!check_features.vk_1_0_features.sparseResidencyBuffer && desired_features_built.vk_1_0_features.sparseResidencyBuffer)
       return false;
    if(!check_features.vk_1_0_features.sparseResidencyImage2D && desired_features_built.vk_1_0_features.sparseResidencyImage2D)
       return false;
    if(!check_features.vk_1_0_features.sparseResidencyImage3D && desired_features_built.vk_1_0_features.sparseResidencyImage3D)
       return false;
    if(!check_features.vk_1_0_features.sparseResidency2Samples && desired_features_built.vk_1_0_features.sparseResidency2Samples)
       return false;
    if(!check_features.vk_1_0_features.sparseResidency4Samples && desired_features_built.vk_1_0_features.sparseResidency4Samples)
       return false;
    if(!check_features.vk_1_0_features.sparseResidency8Samples && desired_features_built.vk_1_0_features.sparseResidency8Samples)
       return false;
    if(!check_features.vk_1_0_features.sparseResidency16Samples && desired_features_built.vk_1_0_features.sparseResidency16Samples)
       return false;
    if(!check_features.vk_1_0_features.sparseResidencyAliased && desired_features_built.vk_1_0_features.sparseResidencyAliased)
       return false;
    if(!check_features.vk_1_0_features.variableMultisampleRate && desired_features_built.vk_1_0_features.variableMultisampleRate)
       return false;
    if(!check_features.vk_1_0_features.inheritedQueries && desired_features_built.vk_1_0_features.inheritedQueries)
       return false;

    // Since Vulkan 1.1
    if(!check_features.vk_1_1_features.storageBuffer16BitAccess && desired_features_built.vk_1_1_features.storageBuffer16BitAccess)
       return false;
    if(!check_features.vk_1_1_features.uniformAndStorageBuffer16BitAccess && desired_features_built.vk_1_1_features.uniformAndStorageBuffer16BitAccess)
       return false;
    if(!check_features.vk_1_1_features.storagePushConstant16 && desired_features_built.vk_1_1_features.storagePushConstant16)
       return false;
    if(!check_features.vk_1_1_features.storageInputOutput16 && desired_features_built.vk_1_1_features.storageInputOutput16)
       return false;
    if(!check_features.vk_1_1_features.multiview && desired_features_built.vk_1_1_features.multiview)
       return false;
    if(!check_features.vk_1_1_features.multiviewGeometryShader && desired_features_built.vk_1_1_features.multiviewGeometryShader)
       return false;
    if(!check_features.vk_1_1_features.multiviewTessellationShader && desired_features_built.vk_1_1_features.multiviewTessellationShader)
       return false;
    if(!check_features.vk_1_1_features.variablePointersStorageBuffer && desired_features_built.vk_1_1_features.variablePointersStorageBuffer)
       return false;
    if(!check_features.vk_1_1_features.variablePointers && desired_features_built.vk_1_1_features.variablePointers)
       return false;
    if(!check_features.vk_1_1_features.protectedMemory && desired_features_built.vk_1_1_features.protectedMemory)
       return false;
    if(!check_features.vk_1_1_features.samplerYcbcrConversion && desired_features_built.vk_1_1_features.samplerYcbcrConversion)
       return false;
    if(!check_features.vk_1_1_features.shaderDrawParameters && desired_features_built.vk_1_1_features.shaderDrawParameters)
       return false;

    // Since Vulkan 1.2
    if(!check_features.vk_1_2_features.samplerMirrorClampToEdge && desired_features_built.vk_1_2_features.samplerMirrorClampToEdge)
       return false;
    if(!check_features.vk_1_2_features.drawIndirectCount && desired_features_built.vk_1_2_features.drawIndirectCount)
       return false;
    if(!check_features.vk_1_2_features.storageBuffer8BitAccess && desired_features_built.vk_1_2_features.storageBuffer8BitAccess)
       return false;
    if(!check_features.vk_1_2_features.uniformAndStorageBuffer8BitAccess && desired_features_built.vk_1_2_features.uniformAndStorageBuffer8BitAccess)
       return false;
    if(!check_features.vk_1_2_features.storagePushConstant8 && desired_features_built.vk_1_2_features.storagePushConstant8)
       return false;
    if(!check_features.vk_1_2_features.shaderBufferInt64Atomics && desired_features_built.vk_1_2_features.shaderBufferInt64Atomics)
       return false;
    if(!check_features.vk_1_2_features.shaderSharedInt64Atomics && desired_features_built.vk_1_2_features.shaderSharedInt64Atomics)
       return false;
    if(!check_features.vk_1_2_features.shaderFloat16 && desired_features_built.vk_1_2_features.shaderFloat16)
       return false;
    if(!check_features.vk_1_2_features.shaderInt8 && desired_features_built.vk_1_2_features.shaderInt8)
       return false;
    if(!check_features.vk_1_2_features.descriptorIndexing && desired_features_built.vk_1_2_features.descriptorIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderInputAttachmentArrayDynamicIndexing && desired_features_built.vk_1_2_features.shaderInputAttachmentArrayDynamicIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderUniformTexelBufferArrayDynamicIndexing && desired_features_built.vk_1_2_features.shaderUniformTexelBufferArrayDynamicIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderStorageTexelBufferArrayDynamicIndexing && desired_features_built.vk_1_2_features.shaderStorageTexelBufferArrayDynamicIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderUniformBufferArrayNonUniformIndexing && desired_features_built.vk_1_2_features.shaderUniformBufferArrayNonUniformIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderSampledImageArrayNonUniformIndexing && desired_features_built.vk_1_2_features.shaderSampledImageArrayNonUniformIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderStorageBufferArrayNonUniformIndexing && desired_features_built.vk_1_2_features.shaderStorageBufferArrayNonUniformIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderStorageImageArrayNonUniformIndexing && desired_features_built.vk_1_2_features.shaderStorageImageArrayNonUniformIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderInputAttachmentArrayNonUniformIndexing && desired_features_built.vk_1_2_features.shaderInputAttachmentArrayNonUniformIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderUniformTexelBufferArrayNonUniformIndexing && desired_features_built.vk_1_2_features.shaderUniformTexelBufferArrayNonUniformIndexing)
       return false;
    if(!check_features.vk_1_2_features.shaderStorageTexelBufferArrayNonUniformIndexing && desired_features_built.vk_1_2_features.shaderStorageTexelBufferArrayNonUniformIndexing)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingUniformBufferUpdateAfterBind && desired_features_built.vk_1_2_features.descriptorBindingUniformBufferUpdateAfterBind)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingSampledImageUpdateAfterBind && desired_features_built.vk_1_2_features.descriptorBindingSampledImageUpdateAfterBind)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingStorageImageUpdateAfterBind && desired_features_built.vk_1_2_features.descriptorBindingStorageImageUpdateAfterBind)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingStorageBufferUpdateAfterBind && desired_features_built.vk_1_2_features.descriptorBindingStorageBufferUpdateAfterBind)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingUniformTexelBufferUpdateAfterBind && desired_features_built.vk_1_2_features.descriptorBindingUniformTexelBufferUpdateAfterBind)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingStorageTexelBufferUpdateAfterBind && desired_features_built.vk_1_2_features.descriptorBindingStorageTexelBufferUpdateAfterBind)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingUpdateUnusedWhilePending && desired_features_built.vk_1_2_features.descriptorBindingUpdateUnusedWhilePending)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingPartiallyBound && desired_features_built.vk_1_2_features.descriptorBindingPartiallyBound)
       return false;
    if(!check_features.vk_1_2_features.descriptorBindingVariableDescriptorCount && desired_features_built.vk_1_2_features.descriptorBindingVariableDescriptorCount)
       return false;
    if(!check_features.vk_1_2_features.runtimeDescriptorArray && desired_features_built.vk_1_2_features.runtimeDescriptorArray)
       return false;
    if(!check_features.vk_1_2_features.samplerFilterMinmax && desired_features_built.vk_1_2_features.samplerFilterMinmax)
       return false;
    if(!check_features.vk_1_2_features.scalarBlockLayout && desired_features_built.vk_1_2_features.scalarBlockLayout)
       return false;
    if(!check_features.vk_1_2_features.imagelessFramebuffer && desired_features_built.vk_1_2_features.imagelessFramebuffer)
       return false;
    if(!check_features.vk_1_2_features.uniformBufferStandardLayout && desired_features_built.vk_1_2_features.uniformBufferStandardLayout)
       return false;
    if(!check_features.vk_1_2_features.shaderSubgroupExtendedTypes && desired_features_built.vk_1_2_features.shaderSubgroupExtendedTypes)
       return false;
    if(!check_features.vk_1_2_features.separateDepthStencilLayouts && desired_features_built.vk_1_2_features.separateDepthStencilLayouts)
       return false;
    if(!check_features.vk_1_2_features.hostQueryReset && desired_features_built.vk_1_2_features.hostQueryReset)
       return false;
    if(!check_features.vk_1_2_features.timelineSemaphore && desired_features_built.vk_1_2_features.timelineSemaphore)
       return false;
    if(!check_features.vk_1_2_features.bufferDeviceAddress && desired_features_built.vk_1_2_features.bufferDeviceAddress)
       return false;
    if(!check_features.vk_1_2_features.bufferDeviceAddressCaptureReplay && desired_features_built.vk_1_2_features.bufferDeviceAddressCaptureReplay)
       return false;
    if(!check_features.vk_1_2_features.bufferDeviceAddressMultiDevice && desired_features_built.vk_1_2_features.bufferDeviceAddressMultiDevice)
       return false;
    if(!check_features.vk_1_2_features.vulkanMemoryModel && desired_features_built.vk_1_2_features.vulkanMemoryModel)
       return false;
    if(!check_features.vk_1_2_features.vulkanMemoryModelDeviceScope && desired_features_built.vk_1_2_features.vulkanMemoryModelDeviceScope)
       return false;
    if(!check_features.vk_1_2_features.vulkanMemoryModelAvailabilityVisibilityChains && desired_features_built.vk_1_2_features.vulkanMemoryModelAvailabilityVisibilityChains)
       return false;
    if(!check_features.vk_1_2_features.shaderOutputViewportIndex && desired_features_built.vk_1_2_features.shaderOutputViewportIndex)
       return false;
    if(!check_features.vk_1_2_features.shaderOutputLayer && desired_features_built.vk_1_2_features.shaderOutputLayer)
       return false;
    if(!check_features.vk_1_2_features.subgroupBroadcastDynamicId && desired_features_built.vk_1_2_features.subgroupBroadcastDynamicId)
       return false;

    return true;
}

bool PhysicalDevice::_find_queue_families(VkSurfaceKHR surface)
{
    uint32_t count{ 0 };
    std::vector<VkQueueFamilyProperties> queue_family_properties;
    vkGetPhysicalDeviceQueueFamilyProperties(_vk_physical_device, &count, nullptr);

    if(count == 0)
    {
        return false;
    }

    _queue_families.reserve(count);
    queue_family_properties.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(_vk_physical_device, &count, queue_family_properties.data());

    for(size_t queue_family_index{ 0 }; queue_family_index < queue_family_properties.size(); ++queue_family_index)
    {
        VkBool32 supports_present;
        vkGetPhysicalDeviceSurfaceSupportKHR(_vk_physical_device, queue_family_index, surface, &supports_present);
        _queue_families.push_back(QueueFamily(queue_family_index, queue_family_properties[queue_family_index], supports_present));

        QueueFamily* queue_family = &_queue_families[queue_family_index];

        if(queue_family->get_properties().queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            _graphics_queue_families.push_back(queue_family);
        }

        if(queue_family->supports_present_queue())
        {
            _present_queue_families.push_back(queue_family);
        }
    }

    return true;
}

bool PhysicalDevice::_find_surface_formats(VkSurfaceKHR surface)
{
    uint32_t count{ 0 };
    vkGetPhysicalDeviceSurfaceFormatsKHR(_vk_physical_device, surface, &count, nullptr);

    if(count == 0)
    {
        return false;
    }

   _vk_surface_formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(_vk_physical_device, surface, &count, _vk_surface_formats.data());

    return true;
}

bool PhysicalDevice::_find_surface_present_modes(VkSurfaceKHR surface)
{
    uint32_t count{ 0 };
    vkGetPhysicalDeviceSurfacePresentModesKHR(_vk_physical_device, surface, &count, nullptr);

    if(count == 0)
    {
        return false;
    }

    _vk_present_modes.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(_vk_physical_device, surface, &count, _vk_present_modes.data());

    return true;
}
