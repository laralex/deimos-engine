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
   //satisfiedDevices.reserve(maybeAllDevices->size());
   // std::copy_if(
   //    maybeAllDevices->begin(), maybeAllDevices->end(),
   //    std::back_inserter(satisfiedDevices), [&](auto& device){
   //       return device.HasFeatures(requiredFeatures);
   // });
   for (auto&& device : *maybeAllDevices) {
      if (device.HasFeatures(requiredFeatures) == false) {
         continue;
      }
      satisfiedDevices.emplace_back(std::move(device));
   }
   return satisfiedDevices;
}

auto PhysicalDevice::GetVendorName() const -> const char* {
   return GetVkPhysicalDeviceVendorStr(_properties.vendorID);
}

auto PhysicalDevice::GetDeviceTypeName() const -> const char* {
   constexpr u32 numTypes = sizeof(VkPhysicalDeviceTypeToStr)/sizeof(void*);
   if (_properties.deviceType >= numTypes) {
      return "UNKNOWN";
   }
   return VkPhysicalDeviceTypeToStr[_properties.deviceType];
}

auto PhysicalDevice::GetSupportedMultisamples() const -> u32 {
   // TODO: implement
   return 1;
}

auto PhysicalDevice::HasFeatures(const VkPhysicalDeviceFeatures& requiredFeatures) const -> b8 {
   return VerifyVkPhysicalFeatures(requiredFeatures, _features);
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
}

} // namespace dei