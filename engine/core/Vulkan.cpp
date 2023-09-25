#include "dei/Vulkan.hpp"

namespace {

constexpr const char* VkPhysicalDeviceTypeToStr[5] = {
   "DEVICE_TYPE_OTHER",
   "DEVICE_TYPE_INTEGRATED_GPU",
   "DEVICE_TYPE_DISCRETE_GPU",
   "DEVICE_TYPE_VIRTUAL_GPU",
   "DEVICE_TYPE_CPU",
};

} // namespace ::

namespace dei {

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

auto GetVulkanPhysicalDevices(VkInstance instance) -> std::optional<u32> {
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
      VkPhysicalDeviceFeatures deviceFeatures;
      vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
      vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

      auto deviceTypeStr = "UNKNOWN";
      if (deviceProperties.deviceType < sizeof(VkPhysicalDeviceTypeToStr)/sizeof(decltype(VkPhysicalDeviceTypeToStr))) {
         deviceTypeStr = VkPhysicalDeviceTypeToStr[deviceProperties.deviceType];
      }
      printf("Detected device type: %s\n", deviceTypeStr);
      printf(" - Name:    %s\n", deviceProperties.deviceName);
      printf(" - Version: %d\n", deviceProperties.driverVersion);
      printf(" - API Version: %d.%d.%d\n",
        VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_API_VERSION_MINOR(deviceProperties.apiVersion),
        VK_API_VERSION_PATCH(deviceProperties.apiVersion));
   }
}

} // namespace dei