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

auto GetVulkanPhysicalDevices(VkInstance instance, const VkPhysicalDeviceFeatures& requiredFeatures) -> std::optional<u32> {
   auto deviceCount = u32{1};
   VkResult res = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
   auto physicalDevices = std::vector<VkPhysicalDevice>(deviceCount);
   res = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
   if (res != VK_SUCCESS && res != VK_INCOMPLETE) {
      // TODO: assert?
      return std::nullopt;
   }

   if (deviceCount == 0) {
      // TODO: assert?
      return std::nullopt;
   }

   // NOTE: is apiVersion >= appInfo.apiVersion? The spec doesn't explain.
   for (const auto& physicalDevice : physicalDevices) {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

      auto deviceTypeStr = "UNKNOWN";
      if (deviceProperties.deviceType < sizeof(VkPhysicalDeviceTypeToStr)/sizeof(void*)) {
         deviceTypeStr = VkPhysicalDeviceTypeToStr[deviceProperties.deviceType];
      }
      printf("Physical device: %s\n", deviceTypeStr);
      printf(" - Vendor       : %s\n", GetVkPhysicalDeviceVendorStr(deviceProperties.vendorID));
      printf(" - Name         : %s\n", deviceProperties.deviceName);
      printf(" - Version      : %zu\n", deviceProperties.driverVersion);
      printf(" - API Version  : %zu.%zu.%zu\n",
        VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_API_VERSION_MINOR(deviceProperties.apiVersion),
        VK_API_VERSION_PATCH(deviceProperties.apiVersion));
      // printf(" - Max Framebuffer : %zu x %zu\n",
      //    deviceProperties.limits.maxFramebufferWidth,
      //    deviceProperties.limits.maxFramebufferHeight);

      VkPhysicalDeviceFeatures actualFeatures;
      vkGetPhysicalDeviceFeatures(physicalDevice, &actualFeatures);
      b8 allFeaturesSatisfied =
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, robustBufferAccess) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, fullDrawIndexUint32) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, imageCubeArray) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, independentBlend) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, geometryShader) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, tessellationShader) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sampleRateShading) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, dualSrcBlend) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, logicOp) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, multiDrawIndirect) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, drawIndirectFirstInstance) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, depthClamp) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, depthBiasClamp) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, fillModeNonSolid) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, depthBounds) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, wideLines) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, largePoints) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, alphaToOne) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, multiViewport) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, samplerAnisotropy) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, textureCompressionETC2) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, textureCompressionASTC_LDR) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, textureCompressionBC) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, occlusionQueryPrecise) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, pipelineStatisticsQuery) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, vertexPipelineStoresAndAtomics) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, fragmentStoresAndAtomics) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderTessellationAndGeometryPointSize) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderImageGatherExtended) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderStorageImageExtendedFormats) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderStorageImageMultisample) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderStorageImageReadWithoutFormat) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderStorageImageWriteWithoutFormat) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderUniformBufferArrayDynamicIndexing) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderSampledImageArrayDynamicIndexing) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderStorageBufferArrayDynamicIndexing) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderStorageImageArrayDynamicIndexing) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderClipDistance) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderCullDistance) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderFloat64) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderInt64) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderInt16) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderResourceResidency) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, shaderResourceMinLod) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseBinding) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidencyBuffer) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidencyImage2D) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidencyImage3D) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidency2Samples) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidency4Samples) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidency8Samples) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidency16Samples) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, sparseResidencyAliased) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, variableMultisampleRate) &&
      DEI_FIELD_SATISFIED(requiredFeatures, actualFeatures, inheritedQueries);
      printf(" - Features satisfied  : %d\n", allFeaturesSatisfied);
   }
}

} // namespace dei