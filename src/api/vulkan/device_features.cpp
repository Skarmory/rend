#include "api/vulkan/device_features.h"

#include "api/vulkan/vulkan_helper_funcs.h"

using namespace rend;

PhysicalDeviceFeatures PhysicalDeviceFeatures::make_device_features(const std::vector<DeviceFeature>& features)
{
    PhysicalDeviceFeatures device_features;
    device_features.vk_1_0_features = vulkan_helpers::gen_vk_1_0_features();
    device_features.vk_1_1_features = vulkan_helpers::gen_vk_1_1_features();
    device_features.vk_1_2_features = vulkan_helpers::gen_vk_1_2_features();

    for(auto feature : features)
    {
        switch(feature)
        {
            case rend::DeviceFeature::ROBUST_BUFFER_ACCESS:
               device_features.vk_1_0_features.robustBufferAccess = true;
               break;
            case rend::DeviceFeature::FULL_DRAW_INDEX_UINT32:
               device_features.vk_1_0_features.fullDrawIndexUint32 = true;
               break;
            case rend::DeviceFeature::IMAGE_CUBE_ARRAY:
               device_features.vk_1_0_features.imageCubeArray = true;
               break;
            case rend::DeviceFeature::INDEPENDENT_BLEND:
               device_features.vk_1_0_features.independentBlend = true;
               break;
            case rend::DeviceFeature::GEOMETRY_SHADER:
               device_features.vk_1_0_features.geometryShader = true;
               break;
            case rend::DeviceFeature::TESSELLATION_SHADER:
               device_features.vk_1_0_features.tessellationShader = true;
               break;
            case rend::DeviceFeature::SAMPLE_RATE_SHADING:
               device_features.vk_1_0_features.sampleRateShading = true;
               break;
            case rend::DeviceFeature::DUAL_SRC_BLEND:
               device_features.vk_1_0_features.dualSrcBlend = true;
               break;
            case rend::DeviceFeature::LOGIC_OP:
               device_features.vk_1_0_features.logicOp = true;
               break;
            case rend::DeviceFeature::MULTI_DRAW_INDIRECT:
               device_features.vk_1_0_features.multiDrawIndirect = true;
               break;
            case rend::DeviceFeature::DRAW_INDIRECT_FIRST_INSTANCE:
               device_features.vk_1_0_features.drawIndirectFirstInstance = true;
               break;
            case rend::DeviceFeature::DEPTH_CLAMP:
               device_features.vk_1_0_features.depthClamp = true;
               break;
            case rend::DeviceFeature::DEPTH_BIAS_CLAMP:
               device_features.vk_1_0_features.depthBiasClamp = true;
               break;
            case rend::DeviceFeature::FILL_MODE_NON_SOLID:
               device_features.vk_1_0_features.fillModeNonSolid = true;
               break;
            case rend::DeviceFeature::DEPTH_BOUNDS:
               device_features.vk_1_0_features.depthBounds = true;
               break;
            case rend::DeviceFeature::WIDE_LINES:
               device_features.vk_1_0_features.wideLines = true;
               break;
            case rend::DeviceFeature::LARGE_POINTS:
               device_features.vk_1_0_features.largePoints = true;
               break;
            case rend::DeviceFeature::ALPHA_TO_ONE:
               device_features.vk_1_0_features.alphaToOne = true;
               break;
            case rend::DeviceFeature::MULTI_VIEWPORT:
               device_features.vk_1_0_features.multiViewport = true;
               break;
            case rend::DeviceFeature::SAMPLER_ANISOTROPY:
               device_features.vk_1_0_features.samplerAnisotropy = true;
               break;
            case rend::DeviceFeature::TEXTURE_COMPRESSION_ETC2:
               device_features.vk_1_0_features.textureCompressionETC2 = true;
               break;
            case rend::DeviceFeature::TEXTURE_COMPRESSION_ASTC_LDR:
               device_features.vk_1_0_features.textureCompressionASTC_LDR = true;
               break;
            case rend::DeviceFeature::TEXTURE_COMPRESSION_BC:
               device_features.vk_1_0_features.textureCompressionBC = true;
               break;
            case rend::DeviceFeature::OCCLUSION_QUERY_PRECISE:
               device_features.vk_1_0_features.occlusionQueryPrecise = true;
               break;
            case rend::DeviceFeature::PIPELINE_STATISTICS_QUERY:
               device_features.vk_1_0_features.pipelineStatisticsQuery = true;
               break;
            case rend::DeviceFeature::VERTEX_PIPELINE_STORES_AND_ATOMICS:
               device_features.vk_1_0_features.vertexPipelineStoresAndAtomics = true;
               break;
            case rend::DeviceFeature::FRAGMENT_STORES_AND_ATOMICS:
               device_features.vk_1_0_features.fragmentStoresAndAtomics = true;
               break;
            case rend::DeviceFeature::SHADER_TESSELLATION_AND_GEOMETRY_POINT_SIZE:
               device_features.vk_1_0_features.shaderTessellationAndGeometryPointSize = true;
               break;
            case rend::DeviceFeature::SHADER_IMAGE_GATHER_EXTENDED:
               device_features.vk_1_0_features.shaderImageGatherExtended = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_IMAGE_EXTENDED_FORMATS:
               device_features.vk_1_0_features.shaderStorageImageExtendedFormats = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_IMAGE_MULTISAMPLE:
               device_features.vk_1_0_features.shaderStorageImageMultisample = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_IMAGE_READ_WITHOUT_FORMAT:
               device_features.vk_1_0_features.shaderStorageImageReadWithoutFormat = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_IMAGE_WRITE_WITHOUT_FORMAT:
               device_features.vk_1_0_features.shaderStorageImageWriteWithoutFormat = true;
               break;
            case rend::DeviceFeature::SHADER_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING:
               device_features.vk_1_0_features.shaderUniformBufferArrayDynamicIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING:
               device_features.vk_1_0_features.shaderSampledImageArrayDynamicIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_BUFFER_ARRAY_DYNAMIC_INDEXING:
               device_features.vk_1_0_features.shaderStorageBufferArrayDynamicIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_IMAGE_ARRAY_DYNAMIC_INDEXING:
               device_features.vk_1_0_features.shaderStorageImageArrayDynamicIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_CLIP_DISTANCE:
               device_features.vk_1_0_features.shaderClipDistance = true;
               break;
            case rend::DeviceFeature::SHADER_CULL_DISTANCE:
               device_features.vk_1_0_features.shaderCullDistance = true;
               break;
            case rend::DeviceFeature::SHADER_FLOAT64:
               device_features.vk_1_0_features.shaderFloat64 = true;
               break;
            case rend::DeviceFeature::SHADER_INT64:
               device_features.vk_1_0_features.shaderInt64 = true;
               break;
            case rend::DeviceFeature::SHADER_INT16:
               device_features.vk_1_0_features.shaderInt16 = true;
               break;
            case rend::DeviceFeature::SHADER_RESOURCE_RESIDENCY:
               device_features.vk_1_0_features.shaderResourceResidency = true;
               break;
            case rend::DeviceFeature::SHADER_RESOURCE_MIN_LOD:
               device_features.vk_1_0_features.shaderResourceMinLod = true;
               break;
            case rend::DeviceFeature::SPARSE_BINDING:
               device_features.vk_1_0_features.sparseBinding = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY_BUFFER:
               device_features.vk_1_0_features.sparseResidencyBuffer = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY_IMAGE2_D:
               device_features.vk_1_0_features.sparseResidencyImage2D = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY_IMAGE3_D:
               device_features.vk_1_0_features.sparseResidencyImage3D = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY2_SAMPLES:
               device_features.vk_1_0_features.sparseResidency2Samples = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY4_SAMPLES:
               device_features.vk_1_0_features.sparseResidency4Samples = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY8_SAMPLES:
               device_features.vk_1_0_features.sparseResidency8Samples = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY16_SAMPLES:
               device_features.vk_1_0_features.sparseResidency16Samples = true;
               break;
            case rend::DeviceFeature::SPARSE_RESIDENCY_ALIASED:
               device_features.vk_1_0_features.sparseResidencyAliased = true;
               break;
            case rend::DeviceFeature::VARIABLE_MULTISAMPLE_RATE:
               device_features.vk_1_0_features.variableMultisampleRate = true;
               break;
            case rend::DeviceFeature::INHERITED_QUERIES:
               device_features.vk_1_0_features.inheritedQueries = true;
               break;

            // Since Vulkan 1.1
            case rend::DeviceFeature::STORAGE_BUFFER_16_BIT_ACCESS:
               device_features.vk_1_1_features.storageBuffer16BitAccess = true;
               break;
            case rend::DeviceFeature::UNIFORM_AND_STORAGE_BUFFER_16_BIT_ACCESS:
               device_features.vk_1_1_features.uniformAndStorageBuffer16BitAccess = true;
               break;
            case rend::DeviceFeature::STORAGE_PUSH_CONSTANT_16:
               device_features.vk_1_1_features.storagePushConstant16 = true;
               break;
            case rend::DeviceFeature::STORAGE_INPUT_OUTPUT_16:
               device_features.vk_1_1_features.storageInputOutput16 = true;
               break;
            case rend::DeviceFeature::MULTIVIEW:
               device_features.vk_1_1_features.multiview = true;
               break;
            case rend::DeviceFeature::MULTIVIEW_GEOMETRY_SHADER:
               device_features.vk_1_1_features.multiviewGeometryShader = true;
               break;
            case rend::DeviceFeature::MULTIVIEW_TESSELLATION_SHADER:
               device_features.vk_1_1_features.multiviewTessellationShader = true;
               break;
            case rend::DeviceFeature::VARIABLE_POINTERS_STORAGE_BUFFER:
               device_features.vk_1_1_features.variablePointersStorageBuffer = true;
               break;
            case rend::DeviceFeature::VARIABLE_POINTERS:
               device_features.vk_1_1_features.variablePointers = true;
               break;
            case rend::DeviceFeature::PROTECTED_MEMORY:
               device_features.vk_1_1_features.protectedMemory = true;
               break;
            case rend::DeviceFeature::SAMPLER_YCBCR_CONVERSION:
               device_features.vk_1_1_features.samplerYcbcrConversion = true;
               break;
            case rend::DeviceFeature::SHADER_DRAW_PARAMETERS:
               device_features.vk_1_1_features.shaderDrawParameters = true;
               break;

            // Since Vulkan 1
            // .2
            case rend::DeviceFeature::SAMPLER_MIRROR_CLAMP_TO_EDGE:
               device_features.vk_1_2_features.samplerMirrorClampToEdge = true;
               break;
            case rend::DeviceFeature::DRAW_INDIRECT_COUNT:
               device_features.vk_1_2_features.drawIndirectCount = true;
               break;
            case rend::DeviceFeature::STORAGE_BUFFER8_BIT_ACCESS:
               device_features.vk_1_2_features.storageBuffer8BitAccess = true;
               break;
            case rend::DeviceFeature::UNIFORM_AND_STORAGE_BUFFER8_BIT_ACCESS:
               device_features.vk_1_2_features.uniformAndStorageBuffer8BitAccess = true;
               break;
            case rend::DeviceFeature::STORAGE_PUSH_CONSTANT8:
               device_features.vk_1_2_features.storagePushConstant8 = true;
               break;
            case rend::DeviceFeature::SHADER_BUFFER_INT64_ATOMICS:
               device_features.vk_1_2_features.shaderBufferInt64Atomics = true;
               break;
            case rend::DeviceFeature::SHADER_SHARED_INT64_ATOMICS:
               device_features.vk_1_2_features.shaderSharedInt64Atomics = true;
               break;
            case rend::DeviceFeature::SHADER_FLOAT16:
               device_features.vk_1_2_features.shaderFloat16 = true;
               break;
            case rend::DeviceFeature::SHADER_INT8:
               device_features.vk_1_2_features.shaderInt8 = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_INDEXING:
               device_features.vk_1_2_features.descriptorIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_INPUT_ATTACHMENT_ARRAY_DYNAMIC_INDEXING:
               device_features.vk_1_2_features.shaderInputAttachmentArrayDynamicIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_UNIFORM_TEXEL_BUFFER_ARRAY_DYNAMIC_INDEXING:
               device_features.vk_1_2_features.shaderUniformTexelBufferArrayDynamicIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_TEXEL_BUFFER_ARRAY_DYNAMIC_INDEXING:
               device_features.vk_1_2_features.shaderStorageTexelBufferArrayDynamicIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_UNIFORM_BUFFER_ARRAY_NON_UNIFORM_INDEXING:
               device_features.vk_1_2_features.shaderUniformBufferArrayNonUniformIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_SAMPLED_IMAGE_ARRAY_NON_UNIFORM_INDEXING:
               device_features.vk_1_2_features.shaderSampledImageArrayNonUniformIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_BUFFER_ARRAY_NON_UNIFORM_INDEXING:
               device_features.vk_1_2_features.shaderStorageBufferArrayNonUniformIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_IMAGE_ARRAY_NON_UNIFORM_INDEXING:
               device_features.vk_1_2_features.shaderStorageImageArrayNonUniformIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_INPUT_ATTACHMENT_ARRAY_NON_UNIFORM_INDEXING:
               device_features.vk_1_2_features.shaderInputAttachmentArrayNonUniformIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_UNIFORM_TEXEL_BUFFER_ARRAY_NON_UNIFORM_INDEXING:
               device_features.vk_1_2_features.shaderUniformTexelBufferArrayNonUniformIndexing = true;
               break;
            case rend::DeviceFeature::SHADER_STORAGE_TEXEL_BUFFER_ARRAY_NON_UNIFORM_INDEXING:
               device_features.vk_1_2_features.shaderStorageTexelBufferArrayNonUniformIndexing = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_UNIFORM_BUFFER_UPDATE_AFTER_BIND:
               device_features.vk_1_2_features.descriptorBindingUniformBufferUpdateAfterBind = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_SAMPLED_IMAGE_UPDATE_AFTER_BIND:
               device_features.vk_1_2_features.descriptorBindingSampledImageUpdateAfterBind = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_STORAGE_IMAGE_UPDATE_AFTER_BIND:
               device_features.vk_1_2_features.descriptorBindingStorageImageUpdateAfterBind = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_STORAGE_BUFFER_UPDATE_AFTER_BIND:
               device_features.vk_1_2_features.descriptorBindingStorageBufferUpdateAfterBind = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_UNIFORM_TEXEL_BUFFER_UPDATE_AFTER_BIND:
               device_features.vk_1_2_features.descriptorBindingUniformTexelBufferUpdateAfterBind = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_STORAGE_TEXEL_BUFFER_UPDATE_AFTER_BIND:
               device_features.vk_1_2_features.descriptorBindingStorageTexelBufferUpdateAfterBind = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING:
               device_features.vk_1_2_features.descriptorBindingUpdateUnusedWhilePending = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_PARTIALLY_BOUND:
               device_features.vk_1_2_features.descriptorBindingPartiallyBound = true;
               break;
            case rend::DeviceFeature::DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT:
               device_features.vk_1_2_features.descriptorBindingVariableDescriptorCount = true;
               break;
            case rend::DeviceFeature::RUNTIME_DESCRIPTOR_ARRAY:
               device_features.vk_1_2_features.runtimeDescriptorArray = true;
               break;
            case rend::DeviceFeature::SAMPLER_FILTER_MINMAX:
               device_features.vk_1_2_features.samplerFilterMinmax = true;
               break;
            case rend::DeviceFeature::SCALAR_BLOCK_LAYOUT:
               device_features.vk_1_2_features.scalarBlockLayout = true;
               break;
            case rend::DeviceFeature::IMAGELESS_FRAMEBUFFER:
               device_features.vk_1_2_features.imagelessFramebuffer = true;
               break;
            case rend::DeviceFeature::UNIFORM_BUFFER_STANDARD_LAYOUT:
               device_features.vk_1_2_features.uniformBufferStandardLayout = true;
               break;
            case rend::DeviceFeature::SHADER_SUBGROUP_EXTENDED_TYPES:
               device_features.vk_1_2_features.shaderSubgroupExtendedTypes = true;
               break;
            case rend::DeviceFeature::SEPARATE_DEPTH_STENCIL_LAYOUTS:
               device_features.vk_1_2_features.separateDepthStencilLayouts = true;
               break;
            case rend::DeviceFeature::HOST_QUERY_RESET:
               device_features.vk_1_2_features.hostQueryReset = true;
               break;
            case rend::DeviceFeature::TIMELINE_SEMAPHORE:
               device_features.vk_1_2_features.timelineSemaphore = true;
               break;
            case rend::DeviceFeature::BUFFER_DEVICE_ADDRESS:
               device_features.vk_1_2_features.bufferDeviceAddress = true;
               break;
            case rend::DeviceFeature::BUFFER_DEVICE_ADDRESS_CAPTURE_REPLAY:
               device_features.vk_1_2_features.bufferDeviceAddressCaptureReplay = true;
               break;
            case rend::DeviceFeature::BUFFER_DEVICE_ADDRESS_MULTI_DEVICE:
               device_features.vk_1_2_features.bufferDeviceAddressMultiDevice = true;
               break;
            case rend::DeviceFeature::VULKAN_MEMORY_MODEL:
               device_features.vk_1_2_features.vulkanMemoryModel = true;
               break;
            case rend::DeviceFeature::VULKAN_MEMORY_MODEL_DEVICE_SCOPE:
               device_features.vk_1_2_features.vulkanMemoryModelDeviceScope = true;
               break;
            case rend::DeviceFeature::VULKAN_MEMORY_MODEL_AVAILABILITY_VISIBILITY_CHAINS:
               device_features.vk_1_2_features.vulkanMemoryModelAvailabilityVisibilityChains = true;
               break;
            case rend::DeviceFeature::SHADER_OUTPUT_VIEWPORT_INDEX:
               device_features.vk_1_2_features.shaderOutputViewportIndex = true;
               break;
            case rend::DeviceFeature::SHADER_OUTPUT_LAYER:
               device_features.vk_1_2_features.shaderOutputLayer = true;
               break;
            case rend::DeviceFeature::SUBGROUP_BROADCAST_DYNAMIC_ID:
               device_features.vk_1_2_features.subgroupBroadcastDynamicId = true;
               break;
        }
    }

    return device_features;
}
