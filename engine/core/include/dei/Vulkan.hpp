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

auto GetVulkanPhysicalDevices(VkInstance instance, const VkPhysicalDeviceFeatures& requiredFeatures) -> std::optional<u32>;

} // namespace dei