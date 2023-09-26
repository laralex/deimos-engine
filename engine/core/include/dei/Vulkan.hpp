#pragma once

#include "dei/Prelude.hpp"

#include <vector>

namespace dei::render {

struct VulkanContext {
   VkInstance instance;
	VkSurfaceKHR windowSurface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	// VkSemaphore imageAvailableSemaphore;
	// VkSemaphore renderingFinishedSemaphore;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkCommandPool commandPool;
   std::vector<VkCommandBuffer> presentCommandBuffers;
};

auto CreateVulkanInstance(const char** requiredExtensions, u32 requiredExtensionsCount) -> VkInstance;

struct PhysicalDevice {
	VkPhysicalDevice Device;
	VkPhysicalDeviceFeatures Features;
	VkPhysicalDeviceProperties Properties;
	const char* VendorAsString;
	const char* DeviceTypeAsString;
};

auto GetVulkanPhysicalDevices(VkInstance instance) -> std::optional<std::vector<PhysicalDevice>>;
auto GetVulkanPhysicalDevices(VkInstance instance, const VkPhysicalDeviceFeatures& requiredFeatures) -> std::optional<std::vector<PhysicalDevice>>;

} // namespace dei