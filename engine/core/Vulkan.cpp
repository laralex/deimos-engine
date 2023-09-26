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

auto GetVulkanPhysicalDevices(VkInstance instance) -> std::optional<std::vector<PhysicalDevice>> {
   auto numAllDevices = u32{1};
   VkResult res = vkEnumeratePhysicalDevices(instance, &numAllDevices, nullptr);
   auto physicalDevices = std::vector<VkPhysicalDevice>(numAllDevices);
   res = vkEnumeratePhysicalDevices(instance, &numAllDevices, physicalDevices.data());
   if ((res != VK_SUCCESS && res != VK_INCOMPLETE) || numAllDevices == 0) {
      // TODO: assert?
      return std::nullopt;
   }

   auto physicalDeviceInfos = std::vector<PhysicalDevice>(numAllDevices);
   for (int i = 0; i < physicalDevices.size(); ++i) {
      // NOTE: is apiVersion >= appInfo.apiVersion? The spec doesn't explain.
      auto deviceProperties = VkPhysicalDeviceProperties{};
      vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
      auto deviceFeatures = VkPhysicalDeviceFeatures{};
      vkGetPhysicalDeviceFeatures(physicalDevices[i], &deviceFeatures);
      physicalDeviceInfos[i] = PhysicalDevice {};
      physicalDeviceInfos[i].Device = physicalDevices[i];
      physicalDeviceInfos[i].Features = deviceFeatures;
      physicalDeviceInfos[i].Properties = deviceProperties;
      physicalDeviceInfos[i].VendorAsString = GetVkPhysicalDeviceVendorStr(deviceProperties.vendorID);

      physicalDeviceInfos[i].DeviceTypeAsString = "UNKNOWN";
      if (deviceProperties.deviceType < sizeof(VkPhysicalDeviceTypeToStr)/sizeof(void*)) {
         physicalDeviceInfos[i].DeviceTypeAsString = VkPhysicalDeviceTypeToStr[deviceProperties.deviceType];
      }
   }
   return physicalDeviceInfos;
}

auto GetVulkanPhysicalDevices(VkInstance instance, const VkPhysicalDeviceFeatures& requiredFeatures) -> std::optional<std::vector<PhysicalDevice>> {
   auto maybeAllDevices = GetVulkanPhysicalDevices(instance);
   if (maybeAllDevices == std::nullopt) {
      return std::nullopt;
   }
   auto& allDevices = *maybeAllDevices;
   auto satisfiedDevices = std::vector<PhysicalDevice>(allDevices.size());
   auto numSatisfiedDevices = u32{0};
   for (const auto& deviceInfo : allDevices) {
      printf("Physical device: %s\n", deviceInfo.DeviceTypeAsString);
      printf(" - Vendor       : %s\n", deviceInfo.VendorAsString);
      printf(" - Name         : %s\n", deviceInfo.Properties.deviceName);
      printf(" - Version      : %zu\n", deviceInfo.Properties.driverVersion);
      printf(" - API Version  : %zu.%zu.%zu\n",
        VK_API_VERSION_MAJOR(deviceInfo.Properties.apiVersion),
        VK_API_VERSION_MINOR(deviceInfo.Properties.apiVersion),
        VK_API_VERSION_PATCH(deviceInfo.Properties.apiVersion));
      printf(" - Max Framebuffer : %zu x %zu\n",
         deviceInfo.Properties.limits.maxFramebufferWidth,
         deviceInfo.Properties.limits.maxFramebufferHeight);

      b8 allFeaturesSatisfied = VerifyVkPhysicalFeatures(requiredFeatures, deviceInfo.Features);
      printf(" - Features satisfied  : %d\n", allFeaturesSatisfied);

      if (allFeaturesSatisfied) {
         satisfiedDevices[numSatisfiedDevices] = deviceInfo;
         ++numSatisfiedDevices;
      }
   }
   satisfiedDevices.resize(numSatisfiedDevices);
   return satisfiedDevices;
}

} // namespace dei