#include "dei/Vulkan.hpp"

#define DEI_FIELD_EQ(LHS, RHS, FIELD) (##LHS.##FIELD == ##RHS.##FIELD)
#define DEI_FIELD_SATISFIED(REQ,ACTUAL,FIELD) (REQ.FIELD == ACTUAL.FIELD || REQ.FIELD == 0)

namespace {

constexpr const char* VkPhysicalDeviceTypeToStr[5] = {
   "DEVICE_TYPE_OTHER",
   "DEVICE_TYPE_INTEGRATED_GPU",
   "DEVICE_TYPE_DISCRETE_GPU",
   "DEVICE_TYPE_VIRTUAL_GPU",
   "DEVICE_TYPE_CPU",
};

constexpr const char* GetVkPhysicalDeviceVendorStr(u32 vendorId) {
   switch (vendorId)
   {
      case 0x1002: return "AMD";
      case 0x1010: return "ImgTec";
      case 0x10DE: return "NVIDIA";
      case 0x13B5: return "ARM";
      case 0x5143: return "Qualcomm";
      case 0x8086: return "Intel";
   }
   return "VENDOR_UNKNOWN";
}

b8 VerifyVkPhysicalLimits(const VkPhysicalDeviceLimits& required, const VkPhysicalDeviceLimits& actual) {
   b8 result = true;
   result &= required.maxImageDimension1D <= actual.maxImageDimension1D;
   result &= required.maxImageDimension2D <= actual.maxImageDimension2D;
   result &= required.maxImageDimension3D <= actual.maxImageDimension3D;
   result &= required.maxImageDimensionCube <= actual.maxImageDimensionCube;
   result &= required.maxImageArrayLayers <= actual.maxImageArrayLayers;
   result &= required.maxTexelBufferElements <= actual.maxTexelBufferElements;
   result &= required.maxUniformBufferRange <= actual.maxUniformBufferRange;
   result &= required.maxStorageBufferRange <= actual.maxStorageBufferRange;
   result &= required.maxPushConstantsSize <= actual.maxPushConstantsSize;
   result &= required.maxMemoryAllocationCount <= actual.maxMemoryAllocationCount;
   result &= required.maxSamplerAllocationCount <= actual.maxSamplerAllocationCount;
   result &= (required.bufferImageGranularity >= actual.bufferImageGranularity // ge
      || required.bufferImageGranularity == 0);
   result &= required.sparseAddressSpaceSize <= actual.sparseAddressSpaceSize;
   result &= required.maxBoundDescriptorSets <= actual.maxBoundDescriptorSets;
   result &= required.maxPerStageDescriptorSamplers <= actual.maxPerStageDescriptorSamplers;
   result &= required.maxPerStageDescriptorUniformBuffers <= actual.maxPerStageDescriptorUniformBuffers;
   result &= required.maxPerStageDescriptorStorageBuffers <= actual.maxPerStageDescriptorStorageBuffers;
   result &= required.maxPerStageDescriptorSampledImages <= actual.maxPerStageDescriptorSampledImages;
   result &= required.maxPerStageDescriptorStorageImages <= actual.maxPerStageDescriptorStorageImages;
   result &= required.maxPerStageDescriptorInputAttachments <= actual.maxPerStageDescriptorInputAttachments;
   result &= required.maxPerStageResources <= actual.maxPerStageResources;
   result &= required.maxDescriptorSetSamplers <= actual.maxDescriptorSetSamplers;
   result &= required.maxDescriptorSetUniformBuffers <= actual.maxDescriptorSetUniformBuffers;
   result &= required.maxDescriptorSetUniformBuffersDynamic <= actual.maxDescriptorSetUniformBuffersDynamic;
   result &= required.maxDescriptorSetStorageBuffers <= actual.maxDescriptorSetStorageBuffers;
   result &= required.maxDescriptorSetStorageBuffersDynamic <= actual.maxDescriptorSetStorageBuffersDynamic;
   result &= required.maxDescriptorSetSampledImages <= actual.maxDescriptorSetSampledImages;
   result &= required.maxDescriptorSetStorageImages <= actual.maxDescriptorSetStorageImages;
   result &= required.maxDescriptorSetInputAttachments <= actual.maxDescriptorSetInputAttachments;
   result &= required.maxVertexInputAttributes <= actual.maxVertexInputAttributes;
   result &= required.maxVertexInputBindings <= actual.maxVertexInputBindings;
   result &= required.maxVertexInputAttributeOffset <= actual.maxVertexInputAttributeOffset;
   result &= required.maxVertexInputBindingStride <= actual.maxVertexInputBindingStride;
   result &= required.maxVertexOutputComponents <= actual.maxVertexOutputComponents;
   result &= required.maxTessellationGenerationLevel <= actual.maxTessellationGenerationLevel;
   result &= required.maxTessellationPatchSize <= actual.maxTessellationPatchSize;
   result &= required.maxTessellationControlPerVertexInputComponents <= actual.maxTessellationControlPerVertexInputComponents;
   result &= required.maxTessellationControlPerVertexOutputComponents <= actual.maxTessellationControlPerVertexOutputComponents;
   result &= required.maxTessellationControlPerPatchOutputComponents <= actual.maxTessellationControlPerPatchOutputComponents;
   result &= required.maxTessellationControlTotalOutputComponents <= actual.maxTessellationControlTotalOutputComponents;
   result &= required.maxTessellationEvaluationInputComponents <= actual.maxTessellationEvaluationInputComponents;
   result &= required.maxTessellationEvaluationOutputComponents <= actual.maxTessellationEvaluationOutputComponents;
   result &= required.maxGeometryShaderInvocations <= actual.maxGeometryShaderInvocations;
   result &= required.maxGeometryInputComponents <= actual.maxGeometryInputComponents;
   result &= required.maxGeometryOutputComponents <= actual.maxGeometryOutputComponents;
   result &= required.maxGeometryOutputVertices <= actual.maxGeometryOutputVertices;
   result &= required.maxGeometryTotalOutputComponents <= actual.maxGeometryTotalOutputComponents;
   result &= required.maxFragmentInputComponents <= actual.maxFragmentInputComponents;
   result &= required.maxFragmentOutputAttachments <= actual.maxFragmentOutputAttachments;
   result &= required.maxFragmentDualSrcAttachments <= actual.maxFragmentDualSrcAttachments;
   result &= required.maxFragmentCombinedOutputResources <= actual.maxFragmentCombinedOutputResources;
   result &= required.maxComputeSharedMemorySize <= actual.maxComputeSharedMemorySize;
   result &= required.maxComputeWorkGroupCount[0] <= actual.maxComputeWorkGroupCount[0];
   result &= required.maxComputeWorkGroupCount[1] <= actual.maxComputeWorkGroupCount[1];
   result &= required.maxComputeWorkGroupCount[2] <= actual.maxComputeWorkGroupCount[2];
   result &= required.maxComputeWorkGroupInvocations <= actual.maxComputeWorkGroupInvocations;
   result &= required.maxComputeWorkGroupSize[0] <= actual.maxComputeWorkGroupSize[0];
   result &= required.maxComputeWorkGroupSize[1] <= actual.maxComputeWorkGroupSize[1];
   result &= required.maxComputeWorkGroupSize[2] <= actual.maxComputeWorkGroupSize[2];
   result &= required.subPixelPrecisionBits <= actual.subPixelPrecisionBits;
   result &= required.subTexelPrecisionBits <= actual.subTexelPrecisionBits;
   result &= required.mipmapPrecisionBits <= actual.mipmapPrecisionBits;
   result &= required.maxDrawIndexedIndexValue <= actual.maxDrawIndexedIndexValue;
   result &= required.maxDrawIndirectCount <= actual.maxDrawIndirectCount;
   result &= required.maxSamplerLodBias <= actual.maxSamplerLodBias;
   result &= required.maxSamplerAnisotropy <= actual.maxSamplerAnisotropy;
   result &= required.maxViewports <= actual.maxViewports;
   result &= required.maxViewportDimensions[0] <= actual.maxViewportDimensions[0];
   result &= required.maxViewportDimensions[1] <= actual.maxViewportDimensions[1];
   result &= (required.viewportBoundsRange[0] >= actual.viewportBoundsRange[0] // ge
      || std::abs(required.viewportBoundsRange[0]) < 0.000001f);
   result &= required.viewportBoundsRange[1] <= actual.viewportBoundsRange[1];
   result &= required.viewportSubPixelBits <= actual.viewportSubPixelBits;
   result &= (required.minMemoryMapAlignment >= actual.minMemoryMapAlignment // ge
      || required.minMemoryMapAlignment == 0);
   result &= (required.minTexelBufferOffsetAlignment >= actual.minTexelBufferOffsetAlignment // ge
      || required.minTexelBufferOffsetAlignment == 0);
   result &= (required.minUniformBufferOffsetAlignment >= actual.minUniformBufferOffsetAlignment // ge
      || required.minUniformBufferOffsetAlignment == 0);
   result &= (required.minStorageBufferOffsetAlignment >= actual.minStorageBufferOffsetAlignment // ge
      || required.minStorageBufferOffsetAlignment == 0);
   result &= (required.minTexelOffset >= actual.minTexelOffset // ge
      || required.minTexelOffset == 0);
   result &= required.maxTexelOffset <= actual.maxTexelOffset;
   result &= (required.minTexelGatherOffset >= actual.minTexelGatherOffset // ge
      || required.minTexelGatherOffset == 0);
   result &= required.maxTexelGatherOffset <= actual.maxTexelGatherOffset;
   result &= (required.minInterpolationOffset >= actual.minInterpolationOffset // ge
      || std::abs(required.minInterpolationOffset) < 0.000001f);
   result &= required.maxInterpolationOffset <= actual.maxInterpolationOffset;
   result &= (required.subPixelInterpolationOffsetBits >= actual.subPixelInterpolationOffsetBits // ge
      || required.subPixelInterpolationOffsetBits == 0);
   result &= required.maxFramebufferWidth <= actual.maxFramebufferWidth;
   result &= required.maxFramebufferHeight <= actual.maxFramebufferHeight;
   result &= required.maxFramebufferLayers <= actual.maxFramebufferLayers;
   result &= (required.framebufferColorSampleCounts & actual.framebufferColorSampleCounts)
      == required.framebufferColorSampleCounts;
   result &= (required.framebufferDepthSampleCounts & actual.framebufferDepthSampleCounts)
      == required.framebufferDepthSampleCounts;
   result &= (required.framebufferStencilSampleCounts & actual.framebufferStencilSampleCounts)
      == required.framebufferStencilSampleCounts;
   result &= (required.framebufferNoAttachmentsSampleCounts & actual.framebufferNoAttachmentsSampleCounts)
      == required.framebufferNoAttachmentsSampleCounts;
   result &= required.maxColorAttachments <= actual.maxColorAttachments;
   result &= (required.sampledImageColorSampleCounts & actual.sampledImageColorSampleCounts)
      == required.sampledImageColorSampleCounts;
   result &= (required.sampledImageIntegerSampleCounts & actual.sampledImageIntegerSampleCounts)
      == required.sampledImageIntegerSampleCounts;
   result &= (required.sampledImageDepthSampleCounts & actual.sampledImageDepthSampleCounts)
      == required.sampledImageDepthSampleCounts;
   result &= (required.sampledImageStencilSampleCounts & actual.sampledImageStencilSampleCounts)
      == required.sampledImageStencilSampleCounts;
   result &= (required.storageImageSampleCounts & actual.storageImageSampleCounts)
      == required.storageImageSampleCounts;
   result &= required.maxSampleMaskWords <= actual.maxSampleMaskWords;
   result &= (required.timestampComputeAndGraphics == actual.timestampComputeAndGraphics
      || required.timestampComputeAndGraphics == VK_FALSE);
   // NOTE: timestampPeriod has informative meaning,
   // it's advisable to NOT require any particular value of it
   result &= required.timestampPeriod <= actual.timestampPeriod;
   result &= required.maxClipDistances <= actual.maxClipDistances;
   result &= required.maxCullDistances <= actual.maxCullDistances;
   result &= required.maxCombinedClipAndCullDistances <= actual.maxCombinedClipAndCullDistances;
   result &= required.discreteQueuePriorities <= actual.discreteQueuePriorities;
   result &= (required.pointSizeRange[0] >= actual.pointSizeRange[0] // ge
      || std::abs(required.pointSizeRange[0]) < 0.000001f);
   result &= required.pointSizeRange[1] <= actual.pointSizeRange[1];
   result &= (required.lineWidthRange[0] >= actual.lineWidthRange[0] // ge
      || std::abs(required.lineWidthRange[0]) < 0.000001f);
   result &= required.lineWidthRange[1] <= actual.lineWidthRange[1];
   result &= (required.pointSizeGranularity >= actual.pointSizeGranularity // ge
      || std::abs(required.pointSizeGranularity) < 0.000001f);
   result &= (required.lineWidthGranularity >= actual.lineWidthGranularity // ge
      || std::abs(required.lineWidthGranularity) < 0.000001f);
   result &= (required.strictLines == actual.strictLines
      || required.strictLines == VK_FALSE);
   result &= (required.standardSampleLocations == actual.standardSampleLocations
      || required.standardSampleLocations == VK_FALSE);
   return result;
      // FIXME: not sure how to compare against (min or max)
      //  VkDeviceSize          optimalBufferCopyOffsetAlignment;
      //  VkDeviceSize          optimalBufferCopyRowPitchAlignment;
      //  VkDeviceSize          nonCoherentAtomSize;
}

b8 VerifyVkPhysicalFeatures(const VkPhysicalDeviceFeatures& required, const VkPhysicalDeviceFeatures& actual) {
   return DEI_FIELD_SATISFIED(required, actual, robustBufferAccess) &&
         DEI_FIELD_SATISFIED(required, actual, fullDrawIndexUint32) &&
         DEI_FIELD_SATISFIED(required, actual, imageCubeArray) &&
         DEI_FIELD_SATISFIED(required, actual, independentBlend) &&
         DEI_FIELD_SATISFIED(required, actual, geometryShader) &&
         DEI_FIELD_SATISFIED(required, actual, tessellationShader) &&
         DEI_FIELD_SATISFIED(required, actual, sampleRateShading) &&
         DEI_FIELD_SATISFIED(required, actual, dualSrcBlend) &&
         DEI_FIELD_SATISFIED(required, actual, logicOp) &&
         DEI_FIELD_SATISFIED(required, actual, multiDrawIndirect) &&
         DEI_FIELD_SATISFIED(required, actual, drawIndirectFirstInstance) &&
         DEI_FIELD_SATISFIED(required, actual, depthClamp) &&
         DEI_FIELD_SATISFIED(required, actual, depthBiasClamp) &&
         DEI_FIELD_SATISFIED(required, actual, fillModeNonSolid) &&
         DEI_FIELD_SATISFIED(required, actual, depthBounds) &&
         DEI_FIELD_SATISFIED(required, actual, wideLines) &&
         DEI_FIELD_SATISFIED(required, actual, largePoints) &&
         DEI_FIELD_SATISFIED(required, actual, alphaToOne) &&
         DEI_FIELD_SATISFIED(required, actual, multiViewport) &&
         DEI_FIELD_SATISFIED(required, actual, samplerAnisotropy) &&
         DEI_FIELD_SATISFIED(required, actual, textureCompressionETC2) &&
         DEI_FIELD_SATISFIED(required, actual, textureCompressionASTC_LDR) &&
         DEI_FIELD_SATISFIED(required, actual, textureCompressionBC) &&
         DEI_FIELD_SATISFIED(required, actual, occlusionQueryPrecise) &&
         DEI_FIELD_SATISFIED(required, actual, pipelineStatisticsQuery) &&
         DEI_FIELD_SATISFIED(required, actual, vertexPipelineStoresAndAtomics) &&
         DEI_FIELD_SATISFIED(required, actual, fragmentStoresAndAtomics) &&
         DEI_FIELD_SATISFIED(required, actual, shaderTessellationAndGeometryPointSize) &&
         DEI_FIELD_SATISFIED(required, actual, shaderImageGatherExtended) &&
         DEI_FIELD_SATISFIED(required, actual, shaderStorageImageExtendedFormats) &&
         DEI_FIELD_SATISFIED(required, actual, shaderStorageImageMultisample) &&
         DEI_FIELD_SATISFIED(required, actual, shaderStorageImageReadWithoutFormat) &&
         DEI_FIELD_SATISFIED(required, actual, shaderStorageImageWriteWithoutFormat) &&
         DEI_FIELD_SATISFIED(required, actual, shaderUniformBufferArrayDynamicIndexing) &&
         DEI_FIELD_SATISFIED(required, actual, shaderSampledImageArrayDynamicIndexing) &&
         DEI_FIELD_SATISFIED(required, actual, shaderStorageBufferArrayDynamicIndexing) &&
         DEI_FIELD_SATISFIED(required, actual, shaderStorageImageArrayDynamicIndexing) &&
         DEI_FIELD_SATISFIED(required, actual, shaderClipDistance) &&
         DEI_FIELD_SATISFIED(required, actual, shaderCullDistance) &&
         DEI_FIELD_SATISFIED(required, actual, shaderFloat64) &&
         DEI_FIELD_SATISFIED(required, actual, shaderInt64) &&
         DEI_FIELD_SATISFIED(required, actual, shaderInt16) &&
         DEI_FIELD_SATISFIED(required, actual, shaderResourceResidency) &&
         DEI_FIELD_SATISFIED(required, actual, shaderResourceMinLod) &&
         DEI_FIELD_SATISFIED(required, actual, sparseBinding) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidencyBuffer) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidencyImage2D) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidencyImage3D) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidency2Samples) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidency4Samples) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidency8Samples) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidency16Samples) &&
         DEI_FIELD_SATISFIED(required, actual, sparseResidencyAliased) &&
         DEI_FIELD_SATISFIED(required, actual, variableMultisampleRate) &&
         DEI_FIELD_SATISFIED(required, actual, inheritedQueries);
}

auto ParseSampleCountFlags(VkSampleCountFlags flags) -> VkSampleCountFlagBits {
   if (flags & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
   if (flags & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
   if (flags & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
   if (flags & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
   if (flags & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
   if (flags & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
   return VK_SAMPLE_COUNT_1_BIT;
}

inline auto GetMaxFramebufferSamples(const VkPhysicalDeviceLimits& limits, bool ofColor, bool ofDepth, bool ofStencil) -> VkSampleCountFlagBits {
      VkSampleCountFlags counts = {};
   if (ofColor) {
      counts &= limits.framebufferColorSampleCounts;
   }
   if (ofDepth) {
      counts &= limits.framebufferDepthSampleCounts;
   }
   if (ofStencil) {
      counts &= limits.framebufferStencilSampleCounts;
   }
   if (ofStencil) {
      counts &= limits.framebufferNoAttachmentsSampleCounts;
   }
   return ParseSampleCountFlags(counts);
}

} // namespace ::

namespace dei::render {

auto CreateVulkanInstance(const char** requiredExtensions, u32 requiredExtensionsCount) -> VkInstance {
   auto appInfo = VkApplicationInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
   appInfo.pNext = nullptr;
   appInfo.pApplicationName = "Deimos App";
   appInfo.pEngineName = "Deimos Engine";
   appInfo.engineVersion = 1;
   appInfo.applicationVersion = 1;
   appInfo.apiVersion = VK_MAKE_API_VERSION(0,1,3,0);

   auto info = VkInstanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
   info.pNext = nullptr;
   info.pApplicationInfo = &appInfo;
   info.enabledExtensionCount = requiredExtensionsCount;
   info.ppEnabledExtensionNames = requiredExtensions;
   info.enabledLayerCount = 0;
   info.ppEnabledLayerNames = nullptr;
   info.flags = 0; //VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

   auto instance = VkInstance{};
   auto result = vkCreateInstance(&info, nullptr, &instance);

   return instance;
}

auto PhysicalDevice::QueryAll(VkInstance instance) -> std::optional<std::vector<PhysicalDevice>> {
   auto numAllDevices = u32{1};
   VkResult res = vkEnumeratePhysicalDevices(instance, &numAllDevices, nullptr);
   auto rawPhysicalDevices = std::vector<VkPhysicalDevice>(numAllDevices);
   res = vkEnumeratePhysicalDevices(instance, &numAllDevices, rawPhysicalDevices.data());
   if ((res != VK_SUCCESS && res != VK_INCOMPLETE) || numAllDevices == 0) {
      // TODO: assert?
      return std::nullopt;
   }

   // can't preallocate, because no deafult contrustor
   auto physicalDevices = std::vector<PhysicalDevice>{};
   for (int i = 0; i < rawPhysicalDevices.size(); ++i) {
      // NOTE: is apiVersion >= appInfo.apiVersion? The spec doesn't explain.
      const auto& rawDevice = rawPhysicalDevices[i];
      auto deviceProperties = VkPhysicalDeviceProperties{};
      auto deviceFeatures = VkPhysicalDeviceFeatures{};
      vkGetPhysicalDeviceProperties(rawDevice, &deviceProperties);
      vkGetPhysicalDeviceFeatures(rawDevice, &deviceFeatures);
      auto device = PhysicalDevice {};
      device._device = rawDevice;
      device._features = deviceFeatures;
      device._properties = deviceProperties;
      physicalDevices.emplace_back(std::move(device));
   }
   return physicalDevices;
}

auto PhysicalDevice::QueryAll(VkInstance instance, const VkPhysicalDeviceFeatures& requiredFeatures) -> std::optional<std::vector<PhysicalDevice>> {
   auto maybeAllDevices = PhysicalDevice::QueryAll(instance);
   if (maybeAllDevices == std::nullopt) {
      return std::nullopt;
   }
   auto satisfiedDevices = std::vector<PhysicalDevice>{};
   for (auto&& device : *maybeAllDevices) {
      if (device.HasFeatures(requiredFeatures) == false) {
         continue;
      }
      satisfiedDevices.emplace_back(std::move(device));
   }
   return satisfiedDevices;
}

auto PhysicalDevice::GetVendorName() const -> const char* {
   return ::GetVkPhysicalDeviceVendorStr(_properties.vendorID);
}

auto PhysicalDevice::GetDeviceTypeName() const -> const char* {
   constexpr u32 numTypes = sizeof(VkPhysicalDeviceTypeToStr)/sizeof(void*);
   if (_properties.deviceType >= numTypes) {
      return "UNKNOWN";
   }
   return VkPhysicalDeviceTypeToStr[_properties.deviceType];
}

auto PhysicalDevice::GetMaxFramebufferSamples(bool ofColor, bool ofDepth, bool ofStencil) const -> VkSampleCountFlagBits {
   return ::GetMaxFramebufferSamples(_properties.limits, ofColor, ofDepth, ofStencil);
}

auto PhysicalDevice::GetMaxFramebufferSamplesNoAttachments() const -> VkSampleCountFlagBits {
   VkSampleCountFlags counts = _properties.limits.framebufferNoAttachmentsSampleCounts;
   return ::ParseSampleCountFlags(counts);
}

auto PhysicalDevice::HasFeatures(const VkPhysicalDeviceFeatures& requiredFeatures) const -> b8 {
   return ::VerifyVkPhysicalFeatures(requiredFeatures, _features);
}

auto PhysicalDevice::HasLimits(const VkPhysicalDeviceLimits& requiredLimits) const -> b8 {
   return ::VerifyVkPhysicalLimits(requiredLimits, _properties.limits);
}

auto PrintPhysicalDevice(const PhysicalDevice& device) -> void {
   const auto& properties = device.GetProperties();
   printf("Physical device: %s\n", device.GetDeviceTypeName());
   printf(" - Vendor       : %s\n", device.GetVendorName());
   printf(" - Name         : %s\n", properties.deviceName);
   printf(" - Version      : %d\n", properties.driverVersion);
   printf(" - API Version  : %d.%d.%d\n",
      VK_API_VERSION_MAJOR(properties.apiVersion),
      VK_API_VERSION_MINOR(properties.apiVersion),
      VK_API_VERSION_PATCH(properties.apiVersion));
   printf(" - Max Framebuffer : %d x %d\n",
      properties.limits.maxFramebufferWidth,
      properties.limits.maxFramebufferHeight);
   printf(" - Min Texel Offset : %d\n", properties.limits.minTexelOffset);
   printf(" - Min Texel Gather Offset : %d\n", properties.limits.minTexelGatherOffset);
   printf(" - Max Image2D Dimension : %d\n", properties.limits.maxImageDimension2D);
   printf(" - Max Vertex Attributes: %d\n", properties.limits.maxVertexInputAttributes);

   // TODO: can simply convert to int
   u32 maxFramebufferSamples = 1;
   switch (device.GetMaxFramebufferSamples()) {
      case VK_SAMPLE_COUNT_2_BIT: maxFramebufferSamples = 2; break;
      case VK_SAMPLE_COUNT_4_BIT: maxFramebufferSamples = 4; break;
      case VK_SAMPLE_COUNT_8_BIT: maxFramebufferSamples = 8; break;
      case VK_SAMPLE_COUNT_16_BIT: maxFramebufferSamples = 16; break;
      case VK_SAMPLE_COUNT_32_BIT: maxFramebufferSamples = 32; break;
      case VK_SAMPLE_COUNT_64_BIT: maxFramebufferSamples = 64; break;
   }
   printf(" - Max Framebuffer Samples : %d\n", maxFramebufferSamples);
}

} // namespace dei